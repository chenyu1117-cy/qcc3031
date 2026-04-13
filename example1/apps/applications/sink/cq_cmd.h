#ifndef _CQ_CMD_H
#define _CQ_CMD_H

#include <sppc.h>
#include "sink_spp_qualification.h"

extern int is_inquiry_mode;
extern uint16 g_current_pair_index;
extern uint16 g_total_pair_count;

typedef enum {
    BLINK_COMMAND_HEAD,
    BLINK_START_PAIR,     //配对:::DB[addr:12]
    BLINK_PAIR_MODE,     //进入配对模式:::CA
    BLINK_CANCEL_PAIR_MOD,     //取消配对模式:::CB
    BLINK_CONNECT_HFP,       //连接到HFP:::SC[index:配对记录索引号:1]
    BLINK_DISCONNECT_HFP,     //断开HFP:::SE
    BLINK_CONNECT_DEVICE,     //连接设备:::CC[addr:12] hfp+a2dp
    BLINK_DISCONNECT_DEVICE,  //断开设备:::CD  hfp+a2dpsinkDisconnectAllSlc
    BLINK_ACCEPT_INCOMING,   //接听来电:::CE
    BLINK_REJECT_INCOMMMING,   //拒接来电:::CF
    BLINK_FINISH_PHONE,      //结束通话:::CG
    BLINK_REDIAL,            //重拨:::CH
    BLINK_VOICE_DIAL,         //语音拨号:::CI
    BLINK_CANCEL_VOICE_DIAL,   //取消语音拨号:::CJ
    BLINK_VOLUME_UP,          //SPK音量调节:::CK -- Music +
    BLINK_VOLUME_DOWN,        //MIC音量调节:::CL -- Music -
    BLINK_MIC_OPEN_CLOSE,     //麦克风打开/关闭:::CM
    BLINK_VOICE_TO_PHONE,     //语音切换到手机:::TF
    BLINK_VOICE_TO_BLUE,      //语音切换到蓝牙:::CP
    BLINK_VOICE_TRANSFER,     //语音在蓝也和手机之间切换:::CO
    BLINK_HANG_UP_WAIT_PHONE,      //挂断等待来电
    BLINK_HANG_UP_CURRENT_ACCEPT_WAIT,  //挂断当前通话,接听等待来电
    BLINK_HOLD_CURRENT_ACCEPT_WAIT,    //保持当前通话接听等待来电
    BLINK_MEETING_PHONE,          //会议电话
    BLINK_DELETE_PAIR_LIST,       //删除配对记录:::GPD
    BLINK_DIAL,                   //拨打电话:::CW[number]
    BLINK_DTMF,                   //拨打分机号:::CX[DTMF:1]
    BLINK_INQUIRY_HFP_STATUS,     //查询HFP状态:::CY
    BLINK_RESET_BLUE,             //复位蓝牙模块:::CZ
    BLINK_CONNECT_A2DP,            //连接A2Dp:::DC[index:配对记录索引号:1]
    BLINK_DISCONNECT_A2DP,         //断开A2DP:::DA
    BLINK_PLAY_PAUSE_MUSIC,        //播放/暂停音乐:::MA
    BLINK_STOP_MUSIC,              //停止音乐:::MC
    BLINK_NEXT_SOUND,              //下一曲:::MD
    BLINK_PREV_SOUND,              //上一曲:::ME
    BLINK_INQUIRY_AUTO_CONNECT_ACCETP,   //查询自动接听和上电自动连接配置:::MF
    BLINK_SET_AUTO_CONNECT_ON_POWER,    //设置上电自动连接:::MG
    BLINK_UNSET_AUTO_CONNECT_ON_POWER,  //取消上电自动连接:::MH
    BLINK_CONNECT_LAST_AV_DEVICE,      //连接最后一个AV设备:::MI
    BLINK_DISCONNECT_LAST_AV_DEVICE,      //连接最后一个AV设备:::MI
    BLINK_MODIFY_LOCAL_NAME,            //更改LOCAL Name:::MM[name]
    BLINK_READ_LOCAL_NAME,
    BLINK_MODIFY_PIN_CODE,              //更改PIN Code:::MN[code]
    BLINK_SET_AUTO_ANSWER,              //设定自动接听:::MP
    BLINK_UNSET_AUTO_ANSWER,            //取消自动接听:::MQ
    BLINK_FAST_FORWARD,                 //快进:::MR
    BLINK_FAST_BACK,                    //快退:::MT
    BLINK_INQUIRY_A2DP_STATUS,          //查询A2DP状态:::MV
    BLINK_INQUIRY_PAIR_RECORD,          //查询配对记录:::MX
    BLINK_INQUIRY_VERSION_DATE,         //查询版本日期:::MY
    BLINK_SET_SIM_PHONE_BOOK,           //读取SIM电话本:::PA
    BLINK_SET_PHONE_PHONE_BOOK,         //读取手机电话本:::PB
    BLINK_SET_OUT_GOING_CALLLOG,        //读取拨通话记录:::PH
    BLINK_SET_INCOMING_CALLLOG,         //读取已接通话记录:::PI
    BLINK_SET_MISSED_CALLLOG,           //读取未接通话记录:::PJ
    BLINK_SET_RECENT_CALLLOG,           //读取最近的通话记录:::PK
    BLINK_START_DISCOVERY,              //开始查找设备:::SD
    BLINK_STOP_DISCOVERY,               //停止查找设备:::ST
    BLINK_MUSIC_MUTE,                   //禁止蓝牙音乐:::VA
    BLINK_MUSIC_UNMUTE,                 //启用蓝牙音乐:::VB
    BLINK_MUSIC_BACKGROUND,             //蓝牙音乐作为背景音，音量减半:::VC
    BLINK_MUSIC_NORMAL,                 //正常播放:::VD
    BLINK_LOCAL_ADDRESS,                //本机蓝牙地址:::VE
    BLINK_OPP_SEND_FILE,                //通过OPP发送文件给手机:::OS[path]
    BLINK_CONNECT_SPP_ADDRESS,          //连接SPP:::SP[addr:12]
    BLINK_SPP_SEND_DATA,                //发送spp数据:::SG[index:1][data]
    BLINK_SPP_DISCONNECT,               //断开spp:::SH[index:1]
    BLINK_INQUIRY_PLAY_STATUS,          //查询a2dp播放状态:::VI
    BLINK_CONNECT_HID,                  //连接hid:::HC[addr:12]
    BLINK_CONNECT_HID_LAST,             //连接最后一个设备的HID:::HE
    BLINK_DISCONNECT_HID,               //断开hid:::HD
    BLINK_HF_CMD,                  //发送HF命令:::HF[cmd]
    BLINK_PAUSE_MUSIC,            //暂停音乐
    BLINK_INQUIRY_AVRCP_STATUS,    //查询AVRCP状态
    BLINK_INQUIRY_MUSIC_INFO,      //查询Music信息
    BLINK_READ_NEXT_PHONEBOOK_COUNT,  //向下读取n个条目（电话本）
    BLINK_READ_LAST_PHONEBOOK_COUNT,  //向上读取n个条目（电话本）
    BLINK_READ_ALL_PHONEBOOK,         //读取全部条目（电话本）
    BLINK_STOP_PHONEBOOK_DOWN,        //停止电话本下载
    BLINK_PAUSE_PHONEBOOK_DOWN,       //暂停电话本下载
    BLINK_PLAY_PHONEBOOK_DOWN,        //继续电话本下载
    BLINK_INQUIRY_HID_STATUS,         //查询HID状态
    BLINK_SET_TOUCH_RESOLUTION,       //设置车机触摸屏分辨率
    BLINK_HID_ADJUST,                 //触摸屏校屏指令
    BLINK_PAN_CONNECT,                //PAN连接
    BLINK_PAN_DISCONNECT,             //断开PAN
    BLINK_INQUIRY_PAN_STATUS,         //查询pan状态
    BLINK_INQUIRY_DB_ADDR,            //查询本地蓝牙地址
    BLINK_OPEN_BT,                    //打开蓝牙
    BLINK_CLOSE_BT,                   //关闭蓝牙
    BLINK_INQUIRY_CUR_BT_ADDR,         //查询当前连接设备的蓝牙地址
    BLINK_INQUIRY_CUR_BT_NAME,         //查询当前连接设备的蓝牙名字
    BLINK_INQUIRY_SPK_MIC_VAL,         //查询SPK及MIC音量
    BLINK_INQUIRY_SIGNEL_BATTERY_VAL,  //查询电池/信号量
    BLINK_INQUIRY_SPP_STATUS,          //查询SPP状态
    BLINK_MUSIC_VOL_SET,               //设置蓝牙音乐音量
    BLINK_PLAY_MUSIC,                  //播放音乐
    BLINK_SET_OPP_PATH,                //设置opp保存路径
    BLINK_ENTER_TESTMODE,              //测试指令
    BLINK_CONNECT_HFP_BY_DISCOVER_INDEX,  //连接搜索到的设备地址的索引
    BLINK_MIC_OPEN,
    BLINK_MIC_CLOSE,
    BLINK_MSG_LIST_INBOX,            //获取收件箱
    BLINK_MSG_LIST_SENT,            //获取发件箱
    BLINK_MSG_LIST_DELETED,            //获取删除的短信
    BLINK_MSG_GET,            //获取短信 YG[handle]
    BLINK_LE_SCAN,            //BLE扫描
    BLINK_LE_CONNECT,            //BLE连接 LC[addr:12]
    BLINK_LE_DISCONNECT,            //BLE断开 LD[addr:12]
    BLINK_INQUIRY_CUR_BT_INFO,
    BLINK_INQUIRY_LICENSE_STATE,
    BLINK_SUDIO_TRACK_MODE,
    BLINK_CARPLAY_CONNECT,
    BLINK_CARPLAY_DISCONNECT,
    BLINK_READ_CARPLAY_VERSION,
    BLINK_UPGRADE_MODE,
    BLINK_CHECK,
    BLINK_REGISTER_SPP_SERVER,
    BLINK_BAUD_RATE,
    BLINK_BLE_ADVERTISE,
    BLINK_UNKNOWN_1,
    BLINK_UNKNOWN_2,
    BLINK_CMD_NUM
}blink_cmd_t;

typedef enum {
    BLINK_IND_HEAD,
    BLINK_IND_HFP_DISCONNECTED,   //HFP已断开:::IA
    BLINK_IND_HFP_CONNECTED,     //HFP已连接:::IB
    BLINK_IND_CALL_SUCCEED,     //去电:::IC
    BLINK_IND_INCOMING,         //来电:::ID[numberlen:2][number]
    BLINK_IND_SECOND_INCOMING,  //通话中的来电::IE
    BLINK_IND_HANG_UP,          //挂机:::IF
    BLINK_IND_TALKING,          //通话中:::IG
    BLINK_IND_OTHER_INCOMING,            //第三方来电
    BLINK_IND_WAITTING,                   //保持号码
    BLINK_IND_ACCEPTWAIT_HANGUPCUR,       //接听保持挂断当前
    BLINK_IND_HANGUPWAIT,                 //挂断保持的
    BLINK_IND_MEETING,                    //会议模式
    BLINK_IND_RING_START,        //来电铃声开始
    BLINK_IND_RING_STOP,         //来电铃声结束
    BLINK_IND_RING_STATUS,       //正在响铃状态
    BLINK_IND_HF_LOCAL,          //手机接听
    BLINK_IND_HF_REMOTE,         //蓝牙接听
    BLINK_IND_IN_PAIR_MODE,     //进入配对模式:::II
    BLINK_IND_EXIT_PAIR_MODE,   //退出配对模式:::IJ
    BLINK_IND_INCOMING_NAME,          //来电名字显示
    BLINK_IND_OUTGOING_TALKING_NUMBER,   //打出电话或通话中号码
    BLINK_IND_INIT_SUCCEED,              //上电初始化成功:::IS
    BLINK_IND_HOLD_CURRENT_ACCEPT_WAITING,    //保持当前通话,接听等待中的电话
    BLINK_IND_CONNECTING,                    //连接中
    BLINK_IND_MUSIC_PLAYING,                 //音乐 播放中:::MB
    BLINK_IND_MUSIC_STOPPED,                  //音乐停止
    BLINK_IND_VOICE_CONNECTED,                //语音连接建立
    BLINK_IND_VOICE_DISCONNECTED,             //语音连接断开
    BLINK_IND_AUTO_CONNECT_ACCEPT,    //开机自动连接,来电自动接听当前配置:::MF[auto_connect:1][auto_answer:1]
    BLINK_IND_CURRENT_ADDR,                   //当前连接设备地址:::JH[addr:12]
    BLINK_IND_CURRENT_NAME,                  //当前连接设备名称:::SA[name]
    BLINK_IND_HFP_STATUS,                     //当前HFP和a2dp状态:::S[hf_state:1][av_state:1] 1:未连接  3:已连接 4：电话拨出 5：电话打入 6：通话中
    BLINK_IND_AV_STATUS,
    BLINK_IND_VERSION_DATE,         //当前版本号
    BLINK_IND_AVRCP_STATUS,                   //当前AVRCP状态
    BLINK_IND_CURRENT_DEVICE_NAME,      //当前设备名称:::MM[name]
    BLINK_IND_CURRENT_DEVICE_NAME2,      //当前设备名称:::MM[name]
    BLINK_IND_CURRENT_PIN_CODE,         //当前配对密码:::MN[code]
    BLINK_IND_A2DP_CONNECTED,                 //A2DP connected
    BLINK_IND_CURRENT_ADDR_NAME,   //当前连接设备地址
    BLINK_IND_CURRENT_AND_PAIR_LIST,  //当前设备名称 和配对记录
    BLINK_IND_A2DP_DISCONNECTED,                  //A2DP已断开
    BLINK_IND_SET_PHONE_BOOK,                 //设定电话本状态
    BLINK_IND_PHONE_BOOK,       //电话本记录显示:::PB[name][FF][number]
    BLINK_IND_SIM_BOOK,       //电话本记录显示:::PF[name][FF][number]
    BLINK_IND_PHONE_BOOK_DONE,                //下载电话本结束:::PC
    BLINK_IND_SIM_DONE,                       //SIM卡结束
    BLINK_IND_CALLLOG_DONE,                   //下载通话记录结束:::PE
    BLINK_IND_CALLLOG,      //通话记录显示:::PD[type:1][number]
    BLINK_IND_DISCOVERY_START,      //查找到的设备:::SF[addr:12][name]
    BLINK_IND_DISCOVERY,      //查找到的设备:::SF[addr:12][name]
    BLINK_IND_DISCOVERY_NAME_CHANGE,                 //查找结束:::SH
    BLINK_IND_DISCOVERY_DONE,                 //查找结束:::SH
    BLINK_IND_LOCAL_ADDRESS,        //本机蓝牙地址:::IZ[addr:12]
    BLINK_IND_SPP_DATA,  //spp数据:::SPD[index:1][data]
    BLINK_IND_SPP_CONNECT,        //spp连接:::SPC[index:1]
    BLINK_IND_SPP_DISCONNECT,       //spp断开:::SPS[index:1]
    BLINK_IND_OPP_RECEIVED_FILE,        //OPP收到文件开始
    BLINK_IND_OPP_PUSH_SUCCEED,               //OPP发送文件成功
    BLINK_IND_OPP_PUSH_FAILED,                //OPP发送文件失败
    BLINK_IND_HID_CONNECTED,                  //hid连接成功
    BLINK_IND_HID_DISCONNECTED,               //hid断开连接
    BLINK_IND_HID_STATUS,           //hid状态
    BLINK_IND_HID_ADJUST,    //HID校屏
    BLINK_IND_MIC_STATUS,            //打开或关闭咪头
    BLINK_IND_SPK_MIC_VAL,      //当前spk, mic音量
    BLINK_IND_MUSIC_INFO,         //当前播放歌曲信息
    BLINK_IND_PLAYSTATUS_INFO,
    BLINK_IND_SPP_STATUS,  //SPP状态
    BLINK_IND_PAN_DISCONNECT,                 //pan断开
    BLINK_IND_PAN_CONNECT,                    //pan连接成功
    BLINK_IND_PAN_STATUS,           //PAN状态
    BLINK_IND_SIGNAL_BATTERY_VAL,             //手机信号强度/电池电量
    BLINK_IND_PAIR_STATE,           //配对状态
    BLINK_IND_PHONE_MODEL,
    BLINK_IND_UPDATE_SUCCESS,                 //pskey升级完成
    BLINK_IND_A2DP_VOL,                  //a2dp音量
    BLINK_IND_OPP_RECEIVED_SUCCESS,           //OPP收到文件成功
    BLINK_IND_OPP_RECEIVED_FAIL,              //OPP收到文件失败
    BLINK_IND_PROFILE_ENABLED,                //协议开关
    BLINK_IND_MSG_LIST,
    BLINK_IND_MSG_TEXT,
    BLINK_IND_LE_CONNECTED,
    BLINK_IND_LE_DISCONNECTED,
    BLINK_IND_LE_RECEIVE,
    BLINK_IND_DEFAULT_DEVICENAME,
    BLINK_IND_LICENSE_STATE,
    BLINK_IND_UUIDS,
    BLINK_IND_UUID,
    BLINK_IND_UUIDE,
    BLINK_IND_SHUTDOWN,
    BLINK_IND_CARPLAY_VERSION,
    BLINK_IND_A2DP_CODEC,
    BLINK_IND_AMP_OPEN,
    BLINK_IND_AMP_CLOSE,
    BLINK_IND_JUMP_TO_MUSIC,
    BLINK_IND_DEVICE_CLASS,
    BLINK_IND_PB_PHOTOS,
    BLINK_IND_PB_PHOTO,
    BLINK_IND_PB_PHOTOE,
    BLINK_IND_COPS,
    BLINK_IND_COVER_ART_START,
    BLINK_IND_COVER_ART_DATA,
    BLINK_IND_COVER_ART_END,
    BLINK_IND_HICAR_CHANNEL,
    BLINK_IND_NUM
}blink_ind_t;


typedef struct {
    uint8 type;          // 可留作扩展，暂时用0表示AT命令
    char cmd[3];         // 两位命令字母，如"DB"
    char param[256];     // 参数字符串
} recv_t;

#define MAX_UUID_LEN 64
typedef struct {
    uint8 index;
    bdaddr addr;
    char uuid[MAX_UUID_LEN];
    Sink spp_sink; 
} spp_uuid_record_t;

#define MAX_SPP_RECORDS 4
extern spp_uuid_record_t g_spp_uuid_records[MAX_SPP_RECORDS];

extern char* default_commands[BLINK_CMD_NUM];
extern char* default_indicates[BLINK_IND_NUM];

void handle_at_command(recv_t *recv);
bool strToBdaddr(const char *str, bdaddr *addr);

uint16 hex_to_bytes(const char *hex_str, uint16 str_len, uint8 *out_buf, uint16 out_buf_size);

#endif // AT_CMD_ENUM_H
