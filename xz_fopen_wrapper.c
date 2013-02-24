#include "php.h"
#include "php_xz.h"
#include "fopen_wrappers.h"
#include <lzma.h>

struct php_xz_stream_data_t {
  lzma_stream strm;
  size_t in_buf_sz;
  size_t out_buf_sz;
  char *in_buf;
  char *out_buf;
  php_stream *stream;
  int fd;
};

static int php_xz_compress(struct php_xz_stream_data_t *self)
{
  //this function will attempt to consume all bytes in lzma_stream->next_in
  // and write to underlying file
  lzma_stream *strm = &self->strm;
  lzma_action action = LZMA_RUN;
  int wrote = 0;
  int to_write = strm->avail_in;
  fprintf(stderr, "xz_compress called with avail_in: %d\n", strm->avail_in);
  fprintf(stderr, "next_in at %08x\n", strm->next_in);

  while (strm->avail_in > 0) {
    lzma_ret ret = lzma_code(strm, action);
    fprintf(stderr, "lzma_code ret: %d\n", ret);
    size_t write_size = self->out_buf_sz - strm->avail_out;
    wrote += php_stream_write(self->stream, self->out_buf, write_size);
    strm->next_out = self->out_buf;
    strm->avail_out = self->out_buf_sz;
  }

  fprintf(stderr, "Resetting next_in to %08x\n", self->in_buf);
  fprintf(stderr, "avail in for shits and giggles: %d\n", strm->avail_in);
  strm->next_in = self->in_buf;

  return to_write;
}



static int php_xz_init_decoder(struct php_xz_stream_data_t *self)
{
  lzma_stream *strm = &self->strm;
  lzma_ret ret = lzma_stream_decoder(strm, UINT64_MAX, LZMA_CONCATENATED);
  if (ret == LZMA_OK) {
    self->in_buf_sz = XZ_INBUF_SIZE;
    self->out_buf_sz = XZ_OUTBUF_SIZE;
    self->in_buf = emalloc(self->in_buf_sz);
    self->out_buf = emalloc(self->out_buf_sz);
    strm->next_in = self->in_buf;
    strm->avail_in = 0;
    strm->next_out = self->out_buf;
    strm->avail_out = self->out_buf_sz;
    return 1;
  }
  return 0;
}

static int php_xz_init_encoder(struct php_xz_stream_data_t *self)
{
  lzma_stream *strm = &self->strm;
  lzma_options_lzma opt_lzma2;
  if (lzma_lzma_preset(&opt_lzma2, LZMA_PRESET_DEFAULT)) {
    return 0;
  }
  
  lzma_filter filters[] = {
    { .id = LZMA_FILTER_LZMA2, .options = &opt_lzma2 },
    { .id = LZMA_VLI_UNKNOWN, .options = NULL },
  };

  lzma_ret ret = lzma_stream_encoder(strm, filters, LZMA_CHECK_CRC64);

  if (ret == LZMA_OK) {
    self->in_buf_sz = XZ_INBUF_SIZE;
    self->out_buf_sz = XZ_OUTBUF_SIZE;
    self->in_buf = emalloc(self->in_buf_sz);
    self->out_buf = emalloc(self->out_buf_sz);
    strm->avail_in = 0;
    strm->next_in = self->in_buf;
    strm->avail_out = self->out_buf_sz;
    strm->next_out = self->out_buf;
    return 1;
  }
  
  /*const char *msg;
  switch (ret) {
    case LZMA_MEM_ERROR:
      msg = "Memory allocation failed";
      break;
    case LZMA_OPTIONS_ERROR:
      msg = "Specified filter chain is not supported";
      break;
    case LZMA_UNSUPPORTED_CHECK:
      msg = "Specified integrity check is not supported";
      break;
    default:
      msg = "Unknown error, possibly a bug";
      break;
  }
  php_error_docref(NULL TSRMLS_CC, E_WARNING, msg);
  */
  return 0;
}



static size_t php_xziop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
  struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
  lzma_stream *strm = &self->strm;

  size_t to_read = count;
    

  return 0;
}

static size_t php_xziop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
  struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
  int wrote = 0;
  int bytes_consumed = 0;
  fprintf(stderr, "in write: buf, count: %08x, %d\n", buf, count);

  lzma_stream *lz_stream = &self->strm;

  while (count - wrote > self->in_buf_sz - lz_stream->avail_in) {
    fprintf(stderr, "Count - wrote is greater than buf size - avail in\n");
    fprintf(stderr, "Count - wrote: %d\n", count - wrote);
    fprintf(stderr, "wrote: %d\n", wrote);
    fprintf(stderr, "avail_in: %d\n", lz_stream->avail_in);
    //copy as many bytes into next_in buffer as we can
    memcpy((void *)self->in_buf + lz_stream->avail_in, buf + wrote, self->in_buf_sz - lz_stream->avail_in);
    wrote += self->in_buf_sz - lz_stream->avail_in;
    lz_stream->avail_in += self->in_buf_sz - lz_stream->avail_in;
    fprintf(stderr, "avail_in incremented: %d\n", lz_stream->avail_in);
    bytes_consumed = php_xz_compress(self);
    if (bytes_consumed < 0) {
      break;
    }
  }
  fprintf(stderr, "wrote out of loop: %d\n", wrote);

  if (count - wrote > 0) {

    fprintf(stderr, "Copying remaining bytes to next_in.\n");
    memcpy((void *)self->in_buf + lz_stream->avail_in, buf + wrote, count - wrote);

    lz_stream->avail_in += count - wrote;
    fprintf(stderr, "Incrementing avail_in for next go around: %d\n", lz_stream->avail_in);
  }
  return count;
}

static int php_xziop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
  struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
  int ret = EOF;

  lzma_stream *strm = &self->strm;
  if (strm->avail_in > 0) {
    php_stream_flush(stream);
  }
  strm->next_out = self->out_buf;
  strm->avail_out = self->out_buf_sz;
  lzma_action action = LZMA_FINISH;
  lzma_ret lz_ret = lzma_code(strm, action);

  if (lz_ret == LZMA_STREAM_END) {
    fprintf(stderr, "LZMA_STREAM_END\n");
    if (strm->avail_out < self->out_buf_sz) {
      size_t write_size = self->out_buf_sz - strm->avail_out;
      php_stream_write(self->stream, self->out_buf, write_size);
    }

  }

  lzma_end(&self->strm);

  if (close_handle) {
    if (self->fd) {
      close(self->fd);
    }
    if (self->stream) {
      php_stream_close(self->stream);
      self->stream = NULL;
    }
  }
  efree(self);

  return ret;
}

static int php_xziop_flush(php_stream *stream TSRMLS_DC)
{
  struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
  int wrote = php_xz_compress(self);
  fprintf(stderr, "Flush called: %d\n", wrote);
  return 0;
}

php_stream_ops php_stream_xzio_ops = {
  php_xziop_write, php_xziop_read,
  php_xziop_close, php_xziop_flush,
  "XZ",
  NULL, /* seek */
  NULL, /* cast */
  NULL, /* stat */
  NULL /* set_option */
};

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, char *path, char *mode, int options,
    char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
  php_stream *stream = NULL, *innerstream = NULL;
  struct php_xz_stream_data_t *self;

  if (strchr(mode, '+') || strcmp(mode, "rw") == 0) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot open xz stream for reading and writing at the same time.");
    return NULL;
  }

  innerstream = php_stream_open_wrapper_ex(path, mode, STREAM_MUST_SEEK | options | STREAM_WILL_CAST, opened_path, context);

  if (innerstream) {
    int fd;

    if (SUCCESS == php_stream_cast(innerstream, PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS)) {
      self = ecalloc(1, sizeof(*self));
      self->stream = innerstream;
      self->fd = fd;

      stream = php_stream_alloc_rel(&php_stream_xzio_ops, self, 0, mode);
      if (stream) {
        stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;

        if (strcmp(mode, "w") == 0) {
          if (!php_xz_init_encoder(self)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not initialize xz encoder.");
            close(fd);
            php_stream_close(innerstream);
            efree(self);
            php_stream_close(stream);
            return NULL;
          }
        }

        if (strcmp(mode, "r") == 0) {
          if (!php_xz_init_decoder(self)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not initialize xz decoder");
            close(fd);
            php_stream_close(innerstream);
            efree(self);
            php_stream_close(stream);
            return NULL;
          }
        }

        return stream;
      }

      close(fd);

      efree(self);

      php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed opening xz stream");
    }

    php_stream_close(innerstream);
  }
  return NULL;
}


