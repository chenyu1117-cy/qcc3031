
#include "cq_cmd.h"
#include <string.h>
#include "my_uart.h"
#include "sink_scan.h"
#include "sink_inquiry.h"
#include <stdio.h>
#include "connection_no_ble.h"
#include "sink_slc.h"
#include "sink_main_task.h"
#include "sink_avrcp.h"
#include "../../libs/hfp/hfp.h"
#include "sink_pbap.h"
#include <ps.h>
#include "my_ps.h"
#include "sink_debug.h"
#include "sink_devicemanager.h"
#include <bdaddr.h>
#include "sink_private_data.h"

#define PS_LOCAL_NAME       (201)  // 添加这一行
int is_inquiry_mode = 1;//0-搜索  1-连接成功  2-配对
uint16 g_current_pair_index = 0;
uint16 g_total_pair_count = 0;

char* default_commands[BLINK_CMD_NUM] = {
    [BLINK_COMMAND_HEAD] = "AT-",
    [BLINK_START_PAIR] = "DB",     //配对:::DB[addr:12]
    [BLINK_PAIR_MODE] = "CA",     //进入配对模式:::CA
    [BLINK_CANCEL_PAIR_MOD] = "CB",     //取消配对模式:::CB
    [BLINK_CONNECT_HFP] = "SC",       //连接到HFP:::SC[index:配对记录索引号:1]
    [BLINK_DISCONNECT_HFP] = "SE",     //断开HFP:::SE
    [BLINK_CONNECT_DEVICE] = "CC",     //连接设备:::CC[addr:12] / CC[index:1] / CC 连接最近连接的设备
    [BLINK_DISCONNECT_DEVICE] = "CD",  //断开设备:::CD  hfp+a2dp
    [BLINK_ACCEPT_INCOMING] = "CE",   //接听来电:::CE
    [BLINK_REJECT_INCOMMMING] = "CF",   //拒接来电:::CF
    [BLINK_FINISH_PHONE] = "CG",      //结束通话:::CG
    [BLINK_REDIAL] = "CH",            //重拨:::CH
    [BLINK_VOICE_DIAL] = "CI",         //语音拨号:::CI
    [BLINK_CANCEL_VOICE_DIAL] = "CJ",   //取消语音拨号:::CJ
    [BLINK_VOLUME_UP] = "CK",          //SPK音量调节:::CK -- Music +
    [BLINK_VOLUME_DOWN] = "CL",        //MIC音量调节:::CL -- Music -
    [BLINK_MIC_OPEN_CLOSE] = "CM",     //麦克风打开/关闭:::CM
    [BLINK_VOICE_TO_PHONE] = "CN",     //语音切换到手机:::TF
    [BLINK_VOICE_TO_BLUE] = "CP",      //语音切换到蓝牙:::CP
    [BLINK_VOICE_TRANSFER] = "CO",     //语音在蓝牙和手机之间切换:::CO
    [BLINK_HANG_UP_WAIT_PHONE] = "CQ",      //挂断等待来电
    [BLINK_HANG_UP_CURRENT_ACCEPT_WAIT] = "CR",  //挂断当前通话,接听等待来电
    [BLINK_HOLD_CURRENT_ACCEPT_WAIT] = "CS",    //保持当前通话接听等待来电
    [BLINK_MEETING_PHONE] = "CT",          //会议电话
    [BLINK_DELETE_PAIR_LIST] = "CV",       //删除配对记录:::CV
    [BLINK_DIAL] = "CW",                   //拨打电话:::CW[number]
    [BLINK_DTMF] = "CX",                   //拨打分机号:::CX[DTMF:1]
    [BLINK_INQUIRY_HFP_STATUS] = "CY",     //查询HFP状态:::CY
    [BLINK_RESET_BLUE] = "CZ",             //复位蓝牙模块:::CZ
    [BLINK_CONNECT_A2DP] = "DC",            //连接A2Dp:::DC[index:配对记录索引号:1]
    [BLINK_DISCONNECT_A2DP] = "DA",         //断开A2DP:::DA
    [BLINK_PLAY_PAUSE_MUSIC] = "MA",        //播放/暂停音乐:::MA
    [BLINK_STOP_MUSIC] = "MC",              //停止音乐:::MC
    [BLINK_NEXT_SOUND] = "MD",              //下一曲:::MD
    [BLINK_PREV_SOUND] = "ME",              //上一曲:::ME
    [BLINK_INQUIRY_AUTO_CONNECT_ACCETP] = "MF",   //查询自动接听和上电自动连接配置:::MF
    [BLINK_SET_AUTO_CONNECT_ON_POWER] = "MG",    //设置上电自动连接:::MG
    [BLINK_UNSET_AUTO_CONNECT_ON_POWER] = "MH",  //取消上电自动连接:::MH
    [BLINK_CONNECT_LAST_AV_DEVICE] = "MI",      //连接最后一个AV设备:::MI
    [BLINK_DISCONNECT_LAST_AV_DEVICE] = "MJ",   //断开最后一个AV设备:::MJ
    [BLINK_MODIFY_LOCAL_NAME] = "MM",            //更改LOCAL Name:::MM[name]
    [BLINK_READ_LOCAL_NAME] = "QN",            //读取LOCAL Name:::MM[name]
    [BLINK_MODIFY_PIN_CODE] = "MN",              //更改PIN Code:::MN[code]
    [BLINK_SET_AUTO_ANSWER] = "MP",              //设定自动接听:::MP
    [BLINK_UNSET_AUTO_ANSWER] = "MQ",            //取消自动接听:::MQ
    [BLINK_FAST_FORWARD] = "MR",                 //快进:::MR
    [BLINK_FAST_BACK] = "MT",                    //快退:::MT
    [BLINK_INQUIRY_A2DP_STATUS] = "MV",          //查询A2DP状态:::MV
    [BLINK_INQUIRY_PAIR_RECORD] = "MX",          //查询配对记录:::MX
    [BLINK_INQUIRY_VERSION_DATE] = "MY",         //查询版本日期:::MY
    [BLINK_SET_SIM_PHONE_BOOK] = "PK",           //读取SIM电话本:::PA
    [BLINK_SET_PHONE_PHONE_BOOK] = "PA",         //读取手机电话本:::PB
    [BLINK_SET_OUT_GOING_CALLLOG] = "PH",        //读取已拨通话记录:::PH
    [BLINK_SET_INCOMING_CALLLOG] = "PI",         //读取已接通话记录:::PI
    [BLINK_SET_MISSED_CALLLOG] = "PJ",           //读取未接通话记录:::PJ
    [BLINK_SET_RECENT_CALLLOG] = "PK",           //读取最近通话记录:::P
    [BLINK_START_DISCOVERY] = "SD",              //开始查找设备:::SD
    [BLINK_STOP_DISCOVERY] = "ST",               //停止查找设备:::ST
    [BLINK_MUSIC_MUTE] = "VA",                   //禁止蓝牙音乐:::VA
    [BLINK_MUSIC_UNMUTE] = "VB",                 //启用蓝牙音乐:::VB
    [BLINK_MUSIC_BACKGROUND] = "VC",             //蓝牙音乐作为背景音，音量减半:::VC
    [BLINK_MUSIC_NORMAL] = "VD",                 //正常播放:::VD
    [BLINK_LOCAL_ADDRESS] = "VE",                //本机蓝牙地址:::VE
    [BLINK_OPP_SEND_FILE] = "OS",                //通过OPP发送文件给手机:::OS[path]
    [BLINK_CONNECT_SPP_ADDRESS] = "SP",          //连接SPP:::SP[addr:12]
    [BLINK_SPP_SEND_DATA] = "SG",                //发送spp数据:::SG[index:1][data]
    [BLINK_SPP_DISCONNECT] = "SH",               //断开spp:::SH[index:1]
    [BLINK_INQUIRY_PLAY_STATUS] = "VI",          //查询a2dp播放状态:::VI
    [BLINK_CONNECT_HID] = "HC",                  //连接hid:::HC[addr:12]
    [BLINK_CONNECT_HID_LAST] = "HE",             //连接最后一个设备的HID:::HE
    [BLINK_DISCONNECT_HID] = "HD",               //断开hid:::HD
    [BLINK_HF_CMD] = "HF",                  //发送HF命令:::HF[cmd]
    [BLINK_PAUSE_MUSIC] = "MB",            //强制暂停音乐
    [BLINK_INQUIRY_AVRCP_STATUS] = "MO",    //查询AVRCP状态
    [BLINK_INQUIRY_MUSIC_INFO] = "MK",      //查询Music信息
    [BLINK_READ_NEXT_PHONEBOOK_COUNT] = "PC",  //向下读取n个条目（电话本）
    [BLINK_READ_LAST_PHONEBOOK_COUNT] = "PD",  //向上读取n个条目（电话本）
    [BLINK_READ_ALL_PHONEBOOK] = "PX",         //读取全部条目（电话本）
    [BLINK_STOP_PHONEBOOK_DOWN] = "PS",        //停止电话本下载
    [BLINK_PAUSE_PHONEBOOK_DOWN] = "PO",       //暂停电话本下载
    [BLINK_PLAY_PHONEBOOK_DOWN] = "PQ",        //继续电话本下载
    [BLINK_INQUIRY_HID_STATUS] = "HY",         //查询HID状态
    [BLINK_SET_TOUCH_RESOLUTION] = "HJ",       //设置车机触摸屏分辨率
    [BLINK_HID_ADJUST] = "HP",                 //触摸屏校屏指令
    [BLINK_PAN_CONNECT] = "NC",                //PAN连接
    [BLINK_PAN_DISCONNECT] = "ND",             //断开PAN
    [BLINK_INQUIRY_PAN_STATUS] = "NY",         //查询pan状态
    [BLINK_INQUIRY_DB_ADDR] = "DF",            //查询本地蓝牙地址
    [BLINK_OPEN_BT] = "P1",                    //打开蓝牙
    [BLINK_CLOSE_BT] = "P0",                   //关闭蓝牙
    [BLINK_INQUIRY_CUR_BT_ADDR] = "QA",         //查询当前连接设备的蓝牙地址
    [BLINK_INQUIRY_CUR_BT_NAME] = "QB",         //查询当前连接设备的蓝牙名字
    [BLINK_INQUIRY_SPK_MIC_VAL] = "QC",         //查询SPK及MIC音量
    [BLINK_INQUIRY_SIGNEL_BATTERY_VAL] = "QD",  //查询电池/信号量
    [BLINK_INQUIRY_SPP_STATUS] = "SY",          //查询SPP状态
    [BLINK_MUSIC_VOL_SET] = "VF",               //设置蓝牙音乐音量
    [BLINK_PLAY_MUSIC] = "MS",                  //强制播放音乐
    [BLINK_SET_OPP_PATH] = "OP",                //设置opp保存路径
    [BLINK_ENTER_TESTMODE] = "TE",              //测试指令
    [BLINK_CONNECT_HFP_BY_DISCOVER_INDEX] = "PT",  //连接搜索到的设备地址的索引
    [BLINK_MIC_OPEN] = "IO",                   //mic打开
    [BLINK_MIC_CLOSE] = "IC",                  //mic关闭
    [BLINK_MSG_LIST_INBOX] = "RR",            //获取收件箱
    [BLINK_MSG_LIST_SENT] = "RS",            //获取发件箱
    [BLINK_MSG_LIST_DELETED] = "RD",            //获取删除的短信
    [BLINK_MSG_GET] = "YG",            //获取短信 YG[handle]
    [BLINK_LE_SCAN] = "LS",            //BLE扫描
    [BLINK_LE_CONNECT] = "LC",            //BLE连接 LC[addr:12]
    [BLINK_LE_DISCONNECT] = "LD",            //BLE断开 LD[addr:12]
    [BLINK_INQUIRY_CUR_BT_INFO] = "QI",     //查询当前连接设备的蓝牙信息
    [BLINK_INQUIRY_LICENSE_STATE] = "QL",   //检测网络授权状态
    [BLINK_SUDIO_TRACK_MODE] = "ZTM",
    [BLINK_CARPLAY_CONNECT] = "FD",         //carplay已连接
    [BLINK_CARPLAY_DISCONNECT] = "FC",      //carplay已断开
    [BLINK_READ_CARPLAY_VERSION] = "QV",     //请求无线 CarPlay 蓝牙协议版本号
    [BLINK_UPGRADE_MODE] = "UP",
    [BLINK_CHECK] = "KC",
    [BLINK_REGISTER_SPP_SERVER] = "RFREG",
    [BLINK_BLE_ADVERTISE] = "LA",
    [BLINK_BAUD_RATE] = "BA",
    [BLINK_UNKNOWN_1] = "HW",
    [BLINK_UNKNOWN_2] = "YI",
};

char* default_indicates[BLINK_IND_NUM] = {
    [BLINK_IND_HEAD] = "" ,
    [BLINK_IND_HFP_DISCONNECTED] = "IA[addr:12]" ,   //HFP已断开:::IA
    [BLINK_IND_HFP_CONNECTED] = "IB[addr:12]" ,     //HFP已连接:::IB
    [BLINK_IND_CALL_SUCCEED] = "IC[numberlen:2][number]" ,    //去电:::IC
    [BLINK_IND_INCOMING] = "ID[numberlen:2][number]" ,         //来电:::ID[numberlen:2][number]
    [BLINK_IND_SECOND_INCOMING] = "IE[numberlen:2][number]" ,  //通话中的来电::IE
    [BLINK_IND_HANG_UP] = "IF" ,          //挂机:::IF
    [BLINK_IND_TALKING] = "IG[numberlen:2][number]" ,            //通话中:::IG
    [BLINK_IND_OTHER_INCOMING] = "IE[numberlen:2][number]" ,   //第三方来电
    [BLINK_IND_WAITTING] = "IK[numberlen:2][number]" ,          //保持号码
    [BLINK_IND_ACCEPTWAIT_HANGUPCUR] = "IL" ,                   //接听保持挂断当前
    [BLINK_IND_HANGUPWAIT] = "IN" ,                             //挂断保持的
    [BLINK_IND_MEETING] = "IM" ,                             //会议模式
    [BLINK_IND_RING_START] = "VR1" ,        //来电铃声开始
    [BLINK_IND_RING_STOP] = "VR0" ,         //来电铃声结束
    [BLINK_IND_HF_LOCAL] = "T1" ,          //手机接听
    [BLINK_IND_HF_REMOTE] = "T0" ,         //蓝牙接听
    [BLINK_IND_IN_PAIR_MODE] = "II" ,     //进入配对模式:::II
    [BLINK_IND_EXIT_PAIR_MODE] = "IJ" ,   //退出配对模式:::IJ
    [BLINK_IND_INCOMING_NAME] = "IQ" ,          //来电名字显示
    [BLINK_IND_OUTGOING_TALKING_NUMBER] = "IR" ,   //打出电话或通话中号码
    [BLINK_IND_INIT_SUCCEED] = "IS" ,              //上电初始化成功:::IS
    [BLINK_IND_HOLD_CURRENT_ACCEPT_WAITING] = "IT" ,    //保持当前通话,接听等待中的电话
    [BLINK_IND_CONNECTING] = "IV[addr:12]" ,            //连接中
    [BLINK_IND_MUSIC_PLAYING] = "MB" ,                 //音乐 播放中:::MB
    [BLINK_IND_MUSIC_STOPPED] = "MA" ,                  //音乐停止
    [BLINK_IND_VOICE_CONNECTED] = "FS" ,                //语音连接建立
    [BLINK_IND_VOICE_DISCONNECTED] = "FF" ,             //语音连接断开
    [BLINK_IND_AUTO_CONNECT_ACCEPT] = "MF[auto_connect:1][auto_answer:1]" ,    //开机自动连接,来电自动接听当前配置:::MF[auto_connect:1][auto_answer:1]
    [BLINK_IND_CURRENT_ADDR] = "JH[addr:12]" ,                   //当前连接设备地址:::JH[addr:12]
    [BLINK_IND_CURRENT_NAME] = "SA[name]" ,                  //当前连接设备名称:::SA[name]
    [BLINK_IND_HFP_STATUS] = "S[hf_state:1][av_state:1]" ,                     //当前HFP和a2dp状态:::S[hf_state:1][av_state:1] 1:未连接  3:已连接 4：电话拨出 5：电话打入 6：通话中
    [BLINK_IND_AV_STATUS] = "S[hf_state:1][av_state:1]" ,
    [BLINK_IND_VERSION_DATE] = "MW[version]" ,         //当前版本号
    [BLINK_IND_AVRCP_STATUS] = "ML[state:1]" ,                   //当前AVRCP状态
    [BLINK_IND_CURRENT_DEVICE_NAME] = "MM[name]" ,      //当前设备名称:::MM[name]
    [BLINK_IND_CURRENT_DEVICE_NAME2] = "NA[name]" ,      //当前设备名称:::MM[name]
    [BLINK_IND_CURRENT_PIN_CODE] = "MN[code]" ,         //当前配对密码:::MN[code]
    [BLINK_IND_A2DP_CONNECTED] = "MU[addr:12]" ,                 //A2DP 已连接
    [BLINK_IND_CURRENT_ADDR_NAME] = "MX0[addr][name]" ,   //当前连接设备地址
    [BLINK_IND_CURRENT_AND_PAIR_LIST] = "MX[index:1][addr][name]" ,  //当前设备名称 和配对记录
    [BLINK_IND_A2DP_DISCONNECTED] = "MY[addr:12]" ,                  //A2DP已断开
    [BLINK_IND_SET_PHONE_BOOK] = "PA" ,                 //设定电话本状态
    [BLINK_IND_PHONE_BOOK] = "PF[namelen:2][numberlen:2][name][number]" ,       //电话本记录显示:::PB[name][FF][number]
    [BLINK_IND_SIM_BOOK] = "PB[namelen:2][numberlen:2][name][number]" ,       //电话本记录显示:::PF[name][FF][number]
    [BLINK_IND_PHONE_BOOK_DONE] = "PC" ,                //下载电话本结束:::PC
    [BLINK_IND_SIM_DONE] = "PC" ,                       //SIM卡结束
    [BLINK_IND_CALLLOG_DONE] = "PE" ,                   //下载通话记录结束:::PE
    [BLINK_IND_CALLLOG] = "PD[type:1][namelen:2][numberlen:2][datelen:2][name][number][date]" ,      //通话记录显示:::PD[type:1][number]
    [BLINK_IND_DISCOVERY_START] = "QS",
    [BLINK_IND_DISCOVERY] = "SF0[addr:12][name]" ,      //查找到的设备:::SF[addr:12][name]
    [BLINK_IND_DISCOVERY_NAME_CHANGE] = "GU[pair:1][addr:12][name]", //查找到的设备名字发生更改
    [BLINK_IND_DISCOVERY_DONE] = "SH[result:1]" ,                 //查找结束:::SH
    [BLINK_IND_LOCAL_ADDRESS] = "DB[addr:12]" ,        //本机蓝牙地址:::IZ[addr:12]
    [BLINK_IND_SPP_DATA] = "SI[index:1][data]" ,  //spp数据:::SPD[index:1][data]
    [BLINK_IND_SPP_CONNECT] = "SV[index:1][addr:12]" ,        //spp连接:::SPC[index:1]
    [BLINK_IND_SPP_DISCONNECT] = "SS[index:1][addr:12]" ,       //spp断开:::SPS[index:1]
    [BLINK_IND_OPP_RECEIVED_FILE] = "OR[path]" ,        //OPP收到文件开始
    [BLINK_IND_OPP_PUSH_SUCCEED] = "OC" ,               //OPP发送文件成功
    [BLINK_IND_OPP_PUSH_FAILED] = "OF" ,                //OPP发送文件失败
    [BLINK_IND_HID_CONNECTED] = "HB" ,                  //hid连接成功
    [BLINK_IND_HID_DISCONNECTED] = "HA" ,               //hid断开连接
    [BLINK_IND_HID_STATUS] = "HS[status:1]" ,           //hid状态
    [BLINK_IND_HID_ADJUST] = "HP[key:1][x:4][y:4]" ,    //HID校屏
    [BLINK_IND_MIC_STATUS] = "IO[status:1]" ,            //打开或关闭咪头
    [BLINK_IND_SPK_MIC_VAL] = "KI[spk:1][mic:1]" ,      //当前spk, mic音量
    [BLINK_IND_MUSIC_INFO] = "MI[title][FF][artist][FF][play_time][FF][cover_handle]" ,         //当前播放歌曲信息
    [BLINK_IND_PLAYSTATUS_INFO] = "MP[playstatus_data]" ,//当前播放进度
    [BLINK_IND_SPP_STATUS] = "SR[index:1][status:1]" ,  //SPP状态
    [BLINK_IND_PAN_DISCONNECT] = "NA" ,                 //pan断开
    [BLINK_IND_PAN_CONNECT] = "NC" ,                    //pan连接成功
    [BLINK_IND_PAN_STATUS] = "NS[status:1]" ,           //PAN状态
    [BLINK_IND_SIGNAL_BATTERY_VAL] = "PS[signal:2][battery:2]" ,             //手机信号强度/电池电量
    [BLINK_IND_PAIR_STATE] = "P[pairState:1][addr:12]" ,           //配对状态
    [BLINK_IND_PHONE_MODEL] = "PM[phonemodel]" ,
    [BLINK_IND_UPDATE_SUCCESS] = "US" ,                 //pskey升级完成
    [BLINK_IND_A2DP_VOL] = "ME[vol]" ,                  //a2dp音量
    [BLINK_IND_OPP_RECEIVED_SUCCESS] = "OP" ,           //OPP收到文件成功
    [BLINK_IND_OPP_RECEIVED_FAIL] = "OD" ,              //OPP收到文件失败
    [BLINK_IND_PROFILE_ENABLED] = "SX[hf][a2dp][spp][hid][opp][pan][map]000" ,                //协议开关
    [BLINK_IND_MSG_LIST] = "YL[namelen:2][numberlen:2][msglen:3][name][number][msg]" ,
    [BLINK_IND_MSG_TEXT] = "YT[text]" ,
    [BLINK_IND_LE_CONNECTED] = "LC[addr:12]" ,
    [BLINK_IND_LE_DISCONNECTED] = "LD[addr:12]" ,
    [BLINK_IND_LE_RECEIVE] = "LR[data]" ,
    [BLINK_IND_DEFAULT_DEVICENAME] = "CQ131" ,//不要修改，升级需要判断这个名字
    [BLINK_IND_LICENSE_STATE] = "VS[state]",
    [BLINK_IND_UUIDS] = "US",
    [BLINK_IND_UUID] = "UU[uuid]",
    [BLINK_IND_UUIDE] = "UE",
    [BLINK_IND_SHUTDOWN] = "ST",
    [BLINK_IND_CARPLAY_VERSION] = "BV[version]",
    [BLINK_IND_A2DP_CODEC] = "VP[codec:1]",
    [BLINK_IND_AMP_OPEN] = "HB",
    [BLINK_IND_AMP_CLOSE] = "HA",
    [BLINK_IND_JUMP_TO_MUSIC] = "FP",
    [BLINK_IND_DEVICE_CLASS] = "DE[class:6]",
    [BLINK_IND_PB_PHOTOS] = "OS[number]",
    [BLINK_IND_PB_PHOTO] = "OO[photo]",
    [BLINK_IND_PB_PHOTOE] = "OE[number]",
    [BLINK_IND_COPS] = "CS[name]",

    [BLINK_IND_COVER_ART_START] = "BS[packet:2][size:3]",
    [BLINK_IND_COVER_ART_DATA] = "BD[index:1][data][checksum]",
    [BLINK_IND_COVER_ART_END] = "BE",
	
    [BLINK_IND_HICAR_CHANNEL] = "HR[ch35],[ch36]",
};


// 根据两位命令字符串查找对应的枚举值
static blink_cmd_t find_cmd_by_str(const char *cmd_str)
{
    for (int i = 0; i < BLINK_CMD_NUM; i++) {
        if (default_commands[i] && strcmp(default_commands[i], cmd_str) == 0) {
            return (blink_cmd_t)i;
        }
    }
    return BLINK_CMD_NUM; // 未知命令
}

void handle_at_command(recv_t *recv)
{
    blink_cmd_t cmd = find_cmd_by_str(recv->cmd);
    if (cmd == BLINK_CMD_NUM) 
    {
        // 未知命令，发送错误响应
        DEBUG(("unknow-cmd\r\n\r\n"));
        return;
    }

    // 根据命令执行相应操作
    switch (cmd) {
        case BLINK_START_DISCOVERY:// "SD"
            // 执行查询操作
            uart_data_stream_tx_data((const uint8*)"QS\r\n", 6);
            inquiryStart(0);
            break;

        case BLINK_STOP_DISCOVERY:  // "ST"
            // 停止查询操作
            inquiryStop();
            uart_data_stream_tx_data((const uint8*)"SH0\r\n", 5);
            break;

        case BLINK_CONNECT_DEVICE:      // "CC[xx:xx:xx:xx:xx:xx]"
        {
            
            sinkEnableDiscoverable();
            sinkEnableConnectable();
            // 执行配对操作
            bdaddr addr;
            if (strToBdaddr(recv->param, &addr))
            {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "IV%s\r\n",recv->param);
                uart_data_stream_tx_data((const uint8*)buffer, strlen(buffer));
                uart_data_stream_tx_data((const uint8*)"P1\r\n", 6);
                slcConnectDevice(&addr,sink_hfp | sink_a2dp | sink_avrcp);
            }
            else
            {
                uart_data_stream_tx_data((const uint8*)"connect failed\r\n", 16);
            }
            break;
        }
        case BLINK_DISCONNECT_DEVICE:       // "CD"
            // 断开连接，清空pdl
            // 1. 断开所有SLC连接
            sinkDisconnectAllSlc();
            // 2. 清空PDL（配对设备列表）
            deviceManagerRemoveAllDevices();
            /* 发送进入配对模式的事件 */
            MessageSend(&theSink.task, EventSysEnterPairingEmptyPDL, 0);
            uart_data_stream_tx_data((const uint8*)"disconnect\r\n", 12);
            break;

        case BLINK_INQUIRY_PAIR_RECORD:// "MX"
            //查询配对记录
        {
            sink_attributes attributes;
            typed_bdaddr dev_addr;
            uint16 pdl_size = 0;
            
            pdl_size = sinkDataGetPDLSize();
            g_total_pair_count = pdl_size;
            g_current_pair_index = 0;
            
            if(pdl_size > 0)
            {
                is_inquiry_mode = 2; // 设置为配对记录查询模式
                // 获取第一个设备并请求名称
                if(deviceManagerGetIndexedAttributes(0, &attributes, &dev_addr))
                {
                    ConnectionReadRemoteName(&theSink.task, &dev_addr.addr);
                }
            }
            break;
        }

        case BLINK_INQUIRY_CUR_BT_INFO:// "QI"
            //查询当前连接设备信息
            uart_data_stream_tx_data((const uint8*)"Connection status displayed\n", 26);
            break;

        case BLINK_PLAY_PAUSE_MUSIC:  // "MA"        
            //播放/暂停音乐:::MA
            sinkAvrcpPlayPause();
            uart_data_stream_tx_data((const uint8*)"Play/Pause music command sent\n", 28);
            break;
    
        case BLINK_NEXT_SOUND:   // "MD"
            //下一曲:::MD
            sinkAvrcpSkipForward();
            uart_data_stream_tx_data((const uint8*)"Next track command sent\n", 26);
            break;

        case BLINK_PREV_SOUND:    //"ME"              
            //上一曲:::ME
            sinkAvrcpSkipBackward();
            uart_data_stream_tx_data((const uint8*)"Previous track command sent\n", 30);
            break;

        case BLINK_FAST_FORWARD:   // "MR"              
            //快进:::MR
            sinkAvrcpFastForwardPress();
            uart_data_stream_tx_data((const uint8*)"Fast forward command sent\n", 27);
            break;

        case BLINK_FAST_BACK:   // "MT"              
            //快退:::MT
            sinkAvrcpRewindPress();
            uart_data_stream_tx_data((const uint8*)"Fast rewind command sent\n", 28);
            break;

        case BLINK_INQUIRY_MUSIC_INFO:   // "MK"              
            //查询Music信息
            sinkAvrcpRetrieveNowPlayingRequest(0, 0, TRUE);
            uart_data_stream_tx_data((const uint8*)"Music info query sent\n", 23);
            break;
            
        case BLINK_MODIFY_LOCAL_NAME:   // "MM[name]"
            //更改LOCAL Name
            if (recv->param && strlen(recv->param) > 0)
            {
                uint16 nameLength = strlen(recv->param);
                ConnectionChangeLocalName(nameLength, (const uint8*)recv->param);
                uint16 result = psWrite(PS_LOCAL_NAME, (const void*)recv->param, nameLength + 1);
                
                DEBUG(("DEBUG: psWrite result=%d, name=%s, length=%d\n", result, recv->param, nameLength + 1));
                uart_data_stream_tx_data((const uint8*)"Local name changed\r\n", 19);
            } 
            else 
            {
                DEBUG(("ERROR: Missing name parameter\r\n"));
            }
            break;

        case BLINK_ACCEPT_INCOMING:   // "CE"
            //接听来电
            HfpCallAnswerRequest(hfp_primary_link, 1);
            break;

        case BLINK_REJECT_INCOMMMING:   // "CF"
            //拒接来电
            HfpCallAnswerRequest(hfp_primary_link, 0);
            break;

             case BLINK_FINISH_PHONE:    // "CG"
            //结束通话
            HfpCallTerminateRequest(hfp_primary_link);
            break;

        case BLINK_REDIAL:    // "CH"
            //重拨 
            break;

        case BLINK_VOICE_DIAL:    // "CI"
            //语音拨号
            break;

        case BLINK_CANCEL_VOICE_DIAL:    // "CJ"
            //取消语音拨号
            break;

        case BLINK_VOLUME_UP:    // "CK"
            //音量调节:::CK -- Music +
            break;

        case BLINK_VOLUME_DOWN:    // "CL"
            //音量调节:::CL -- Music -
            break;

        case BLINK_MIC_OPEN_CLOSE:    // "CM"
            //麦克风打开/关闭
            break;

        case BLINK_VOICE_TO_PHONE:    // "CN"
            //语音切换到手机
            HfpAudioTransferRequest(hfp_primary_link, hfp_audio_to_ag, sync_all_sco, NULL);
            break;

        case BLINK_VOICE_TO_BLUE:    // "CP"
            //语音切换到蓝牙
            HfpAudioTransferRequest(hfp_primary_link, hfp_audio_to_hfp, sync_all_sco, NULL);

            break;

        case BLINK_VOICE_TRANSFER:    // "CO"
            //语音在蓝牙和手机之间切换
            HfpAudioTransferRequest(hfp_primary_link, hfp_audio_transfer, sync_all_sco, NULL);
            break;

        case BLINK_HANG_UP_WAIT_PHONE:    // "CQ"
            //挂断等待来电
            HfpCallHoldActionRequest(hfp_primary_link, hfp_chld_release_held_reject_waiting, 0);
            break;

        case BLINK_HANG_UP_CURRENT_ACCEPT_WAIT:    // "CR"
            //挂断当前通话,接听等待来电
            HfpCallHoldActionRequest(hfp_primary_link, hfp_chld_release_active_accept_other, 0);
            break;

        case BLINK_HOLD_CURRENT_ACCEPT_WAIT:    // "CS"
            //保持当前通话接听等待来电
            HfpCallHoldActionRequest(hfp_primary_link, hfp_chld_hold_active_accept_other, 0);
            break;

        case BLINK_MEETING_PHONE:    // "CT"
            //会议电话
            HfpCallHoldActionRequest(hfp_primary_link, hfp_chld_add_held_to_multiparty, 0);
            break;

        case BLINK_INQUIRY_HFP_STATUS:    // "CY"
            //查询HFP状态
            HfpCurrentCallsRequest(hfp_primary_link);
            break;

        case BLINK_DTMF:    // "CX"
            //拨打分机号:::CX[DTMF:1]
            HfpDtmfRequest(hfp_primary_link, recv->param[0]);
            break;

        case BLINK_SET_PHONE_PHONE_BOOK:    // "PA"
            //读取手机电话本
            if (pbapConnect(hfp_primary_link)) 
            { 
                pbapSetActivePhonebook(pbap_pb); 
                pbapSetCommand(pbapc_downloading); 
                
                // 延迟发送拉取请求
                MessageSendLater(&theSink.task, PBAPC_APP_PULL_PHONE_BOOK, 0, 500); 
            }

            break;

        case BLINK_SET_OUT_GOING_CALLLOG:    // "PH"
            //读取已拨通话记录
            if (pbapConnect(hfp_primary_link)) 
            { 
                pbapSetActivePhonebook(pbap_och);
                pbapSetCommand(pbapc_downloading);
                
                // 延迟发送拉取请求
                MessageSendLater(&theSink.task, PBAPC_APP_PULL_PHONE_BOOK, 0, 500); 
            }
            break;

        case BLINK_SET_INCOMING_CALLLOG:    // "PI"
            //读取已接通话记录
            if (pbapConnect(hfp_primary_link)) 
            { 
                pbapSetActivePhonebook(pbap_ich);
                pbapSetCommand(pbapc_downloading);
                
                // 延迟发送拉取请求
                MessageSendLater(&theSink.task, PBAPC_APP_PULL_PHONE_BOOK, 0, 500); 
            }
            break;

        case BLINK_SET_MISSED_CALLLOG:    // "PJ"
            //读取未接通话记录
            if (pbapConnect(hfp_primary_link)) 
            { 
                pbapSetActivePhonebook(pbap_mch);
                pbapSetCommand(pbapc_downloading);
                
                // 延迟发送拉取请求
                MessageSendLater(&theSink.task, PBAPC_APP_PULL_PHONE_BOOK, 0, 500); 
            }
            break;


            // ... 添加其他命令处理
        default:
            // 对于没有具体实现的命令，返回OK或ERROR
            break;
    }
}

bool strToBdaddr(const char *str, bdaddr *addr) {
    uint8_t b[6] = {0};
    int idx = 0;
    int nibble = 0;   // 0=等待高4位，1=等待低4位
    uint8_t val = 0;

    while (*str && idx < 6) {
        char c = *str++;
        uint8_t digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else continue;   // 跳过所有非十六进制字符（包括冒号、括号、空格、换行等）

        if (nibble == 0) {
            val = digit << 4;
            nibble = 1;
        } else {
            val |= digit;
            b[idx++] = val;
            nibble = 0;
        }
    }

    if (idx != 6) return FALSE;

    addr->nap = (b[0] << 8) | b[1];
    addr->uap = b[2];
    addr->lap = (b[3] << 16) | (b[4] << 8) | b[5];
    return TRUE;
}
