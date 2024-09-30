#pragma once

#define QRC_QSS(x) ":/Resources/QSS/" + x + ".css"
#define INI_PATH(f) QApplication::applicationDirPath() + "/ini/" + f
#define APP_ICON(i) ":/Resources/MainWindow/app/" + i + ".png"
#define IMG_PATH(p) ":/Resources/MainWindow/emotion/" + p + ".png"
#define HTML_PATH(h) ":/Resources/MainWindow/MsgHtml/" + h + ".html"

#define IMG_SRC "<img src='%1'/>"
#define IMG_QRC "qrc:/Resources/MainWindow/emotion/"
#define IMG_PNG ".png"

#define GROUP_ID_LENGTH 4

#define MSGOBJ_RIGHT "external0"
#define MSGOBJ_LEFT(id) "external_" + id

#define IMG "img"
#define TXT "txt"
#define BR "<br>"
#define MSG_LENGTH 2048

#define HTML_REPLACE "MSG"
#define MYSELF "0"

#define TYPE "type"
#define TYPE_GROUP "0"
#define TYPE_SINGLE "1"

#define SEND "send"
#define RECV "recv"

#define WHAT "what"
#define WHAT_MSG "msg"
#define WHAT_FILE "file"

#define FILE_NAME "name"
#define FILE_LENGTH "len"
#define FILE_CONTENT "content"

enum ButtonType
{
	MIN_CLOSE_BUTTON = 0,    //最小化及关闭按钮
	MIN_MAX_CLOSE_BUTTON,    //最小化、最大化及关闭按钮
	CLOSE_BUTTON             //仅有关闭按钮
};


enum EventType
{
	ET_LoginRequest,    //登录请求事件
	ET_LoginResponse,   //登录响应事件
	ET_LogoutRequest,   //注销请求事件
	ET_LogoutResponse,  //注销响应事件
	ET_HeartBeat,       //心跳事件
	ET_Other
};

#include <cstring>
#define   cHeaderMagic       "QTQQ"
#define   lHeaderMagic       strlen(cHeaderMagic)
#define   lHeaderEventType   sizeof(int16_t)
#define   lHeaderLen         sizeof(int32_t)
#define   lHeaderTotal       (lHeaderMagic + lHeaderEventType + lHeaderLen)