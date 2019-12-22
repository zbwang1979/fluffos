const term = new Terminal({
  rows: 30,
  cols: 80,
  convertEol: true,
  scrollback: 1024,
});

function get_appropriate_ws_url(extra_url) {
  var pcol;
  var u = document.URL;

  /*
   * We open the websocket encrypted if this page came on an
   * https:// url itself, otherwise unencrypted
   */

  if (u.substring(0, 5) === "https") {
    pcol = "wss://";
    u = u.substr(8);
  } else {
    pcol = "ws://";
    if (u.substring(0, 4) === "http")
      u = u.substr(7);
  }

  u = u.split("/");

  /* + "/xxx" bit is for IE10 workaround */

  return pcol + u[0] + "/" + extra_url;
}

function new_ws(urlpath, protocol) {
  if (typeof MozWebSocket != "undefined")
    return new MozWebSocket(urlpath, protocol);

  return new WebSocket(urlpath, protocol);
}

document.addEventListener("DOMContentLoaded", function () {
  term.open(document.getElementById('terminal'));

  ws = new_ws(get_appropriate_ws_url(""), "ascii");
  const attachAddon = new AttachAddon.AttachAddon(ws);
  term.loadAddon(attachAddon);

  try {
    ws.onopen = function () {
      term.write("\r\n======== Connected.\r\n");
    };
    ws.onclose = function () {
      term.write("\r\n======== Connection Lost.\r\n");
    };
  } catch (exception) {
    alert("<p>Error " + exception);
  }

}, false);
