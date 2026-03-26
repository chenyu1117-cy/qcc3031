#include "my_ps.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sink_debug.h"

/**
 * 安全写入数据到持久存储，确保对齐
 * @param key 存储键值
 * @param data 要存储的数据
 * @param bytes 数据大小（字节）
 * @return 实际写入的字数（uint16），失败返回0
 */
uint16 psWrite(uint16 key, const void *data, uint32 bytes) 
{
    // 计算需要的字数
    uint16 words = (bytes + 1) / 2; // 向上取整到最近的字
    
    // 检查数据是否对齐
    if (((uintptr_t)data % 2) != 0) 
    {
        // 数据未对齐，需要临时对齐缓冲区
        uint16 *alignedBuffer = (uint16 *)malloc(words * sizeof(uint16));
        if (!alignedBuffer) 
        {
            DEBUG(("DEBUG: psWrite malloc failed\n"));
            return 0;
        }
        
        // 复制数据到对齐缓冲区
        memcpy(alignedBuffer, data, bytes);
        
        // 写入对齐的数据
        uint16 result = PsStore(key, alignedBuffer, words);
        DEBUG(("DEBUG: psWrite PsStore result=%d, key=%d, words=%d\n", result, key, words));
        
        // 释放临时缓冲区
        free(alignedBuffer);
        
        return result;
    } 
    else 
    {
        // 数据已经对齐，直接写入
        uint16 result = PsStore(key, data, words);
        DEBUG(("DEBUG: psWrite PsStore result=%d, key=%d, words=%d\n", result, key, words));
        return result;
    }
}

/**
 * 从持久存储读取数据，确保对齐
 * @param key 存储键值
 * @param data 存储读取数据的缓冲区
 * @param bytes 缓冲区大小（字节）
 * @return 实际读取的字节数，失败返回0
 */
uint32 psRead(uint16 key, void *data, uint32 bytes) 
{
    // 计算需要的字数
    uint16 words = (bytes + 1) / 2; // 向上取整到最近的字
    DEBUG(("DEBUG: psRead key=%d, bytes=%d, words=%d\n", key, bytes, words));
    
    // 检查数据是否对齐
    if (((uintptr_t)data % 2) != 0) 
    {
        // 数据未对齐，需要临时对齐缓冲区
        uint16 *alignedBuffer = (uint16 *)malloc(words * sizeof(uint16));
        if (!alignedBuffer) 
        {
            DEBUG(("DEBUG: psRead malloc failed\n"));
            return 0;
        }
        
        // 读取到对齐缓冲区
        uint16 result = PsRetrieve(key, alignedBuffer, words);
        DEBUG(("DEBUG: psRead PsRetrieve result=%d\n", result));
        
        if (result > 0)
        {
            // 复制数据到用户缓冲区
            uint32 actualBytes = result * 2;
            if (actualBytes > bytes) {
                actualBytes = bytes;
            }
            memcpy(data, alignedBuffer, actualBytes);
            
            // 释放临时缓冲区
            free(alignedBuffer);
            
            DEBUG(("DEBUG: psRead success, actualBytes=%d\n", actualBytes));
            return actualBytes;
        } else 
        {
            // 读取失败
            free(alignedBuffer);
            DEBUG(("DEBUG: psRead PsRetrieve failed\n"));
            return 0;
        }
    } 
    else 
    {
        // 数据已经对齐，直接读取
        uint16 result = PsRetrieve(key, data, words);
        DEBUG(("DEBUG: psRead PsRetrieve result=%d\n", result));
        if (result > 0) {
            DEBUG(("DEBUG: psRead success, actualBytes=%d\n", result * 2));
        } else {
            DEBUG(("DEBUG: psRead PsRetrieve failed\n"));
        }
        return result * 2; // 转换为字节数
    }
}

