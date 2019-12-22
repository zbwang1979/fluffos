#include <libwebsockets.h>

/* one of these is created for each client connecting to us */
struct ws_ascii_session {
  struct lws *wsi;
  struct interactive_t *user;

  struct evbuffer *buffer;
};

int ws_ascii_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in,
                      size_t len);
