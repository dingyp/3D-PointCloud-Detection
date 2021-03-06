
#pragma once

#define WM_WNDMESSAGE 0x300
//选取
#define WM_IM_SELECT_BEGIN     (WM_USER + WM_WNDMESSAGE + 1)
#define WM_IM_SELECT_END       (WM_USER + WM_WNDMESSAGE + 3)

//普通旋转
#define WM_IM_NROTATE_BEGIN    (WM_USER + WM_WNDMESSAGE + 4)
#define WM_IM_NROTATE_PROGRESS (WM_USER + WM_WNDMESSAGE + 5)
#define WM_IM_NROTATE_END      (WM_USER + WM_WNDMESSAGE + 6)

//包围盒旋转
#define WM_IM_BOXROTATE_BEGIN     (WM_USER + WM_WNDMESSAGE + 7)
#define WM_IM_BOXROTATE_PROGRESS (WM_USER + WM_WNDMESSAGE + 8)
#define WM_IM_BOXROTATE_END      (WM_USER + WM_WNDMESSAGE + 9)

//平移
#define WM_IM_TRANSLATE_BEGIN    (WM_USER + WM_WNDMESSAGE + 10)
#define WM_IM_TRANSLATE_PROGRESS (WM_USER + WM_WNDMESSAGE + 11)
#define WM_IM_TRANSLATE_END      (WM_USER + WM_WNDMESSAGE + 12)

//缩放
#define WM_IM_SCALE_PROGRESS    (WM_USER + WM_WNDMESSAGE + 13)