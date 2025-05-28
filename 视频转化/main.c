#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// #include "inflate.h"
int lz77_decompress(const uint8_t *input, size_t input_size, uint8_t *output,
                    size_t output_size) {
  const uint8_t *input_end = input + input_size;
  uint8_t *output_end = output + output_size;
  uint8_t *output_ptr = output;

  while (input < input_end) {
    uint8_t flag = *input++;
    if (flag < 0x80) {
      // Literal run
      size_t len = flag & 0x7F;
      if (input + len > input_end || output_ptr + len > output_end) {
        return -1; // Overflow check
      }
      for (size_t i = 0; i < len; i++) {
        *output_ptr++ = *input++;
      }
    } else {
      // Back reference
      if (input + 2 > input_end) {
        return -1; // Input overflow
      }
      uint16_t offset = ((flag & 0x7F) << 8) | *input++;
      offset += 1; // Offset starts from 1
      uint8_t length = *input++;
      if (length == 0 || offset > (size_t)(output_ptr - output)) {
        return -1; // Invalid length or offset
      }
      uint8_t *src = output_ptr - offset;
      if (output_ptr + length > output_end) {
        return -1; // Output overflow
      }
      for (uint8_t i = 0; i < length; i++) {
        *output_ptr++ = *src++;
      }
    }
  }
  return (int)(output_ptr - output);
}
//这个就相当于读取flash 只不过我这里是读取本地文件
void read_bytes(unsigned int pos, unsigned int size, unsigned char *buffer) {
  static FILE *fp = NULL;
  if (fp == NULL) {
    fp = fopen("goodapple.bin", "rb");
    if (fp == NULL) {
      perror("Failed to open file");
      return;
    }
  }
  fseek(fp, pos, SEEK_SET);
  size_t bytes_read = fread(buffer, 1, size, fp);
  if (bytes_read != size) {
    perror("Failed to read bytes");
  }
}
void display_frame(unsigned char *frame) {
  for (int i = 0; i < 114 * 64; i++) {
    bool pixel = (frame[i / 8] & (1 << (i % 8))) != 0;
    printf("%c", pixel ? '#' : ' ');
    if ((i + 1) % 114 == 0) {
      printf("\n");
    }
  }
}
int main() {

  unsigned char frame_raw_buffer[920];
  unsigned char frame_buffer[920];

  unsigned int frame_num = 0;
  
  read_bytes(0, 4, (unsigned char *)&frame_num);
  unsigned int picture_start=4+(frame_num+1)*4;
  
  printf("总共%d帧\n", frame_num);

  for (int frame_index = 0; frame_index < frame_num; frame_index++) {
    // 通过帧序数定位帧位置
    unsigned int frame_start;
    unsigned int frame_end;
    read_bytes(4 + 4 * frame_index, 4, (unsigned char *)&frame_start);
    read_bytes(4 + 4 * (frame_index + 1), 4, (unsigned char *)&frame_end);
    unsigned int length = frame_end - frame_start; // 计算帧长度
    // 查看下一个指针 来计算帧大小
    printf("第%d帧: 起始位置 %d, 长度 %d\n", frame_index + 1, frame_start,
           length);
    // // 读取帧数据
    read_bytes(picture_start+frame_start, length, frame_raw_buffer);
    // int s = inflate_zlib(frame_raw_buffer, length, frame_buffer, sizeof(frame_buffer));
    int s = lz77_decompress(frame_raw_buffer, length, frame_buffer, sizeof(frame_buffer));
    if (s < 0) {
      printf("解压缩失败: 错误代码 %d\n", s);
      continue;
    }else{
      printf("解压缩成功: 解压后大小 %d\n", s);
      display_frame(frame_buffer);
      printf("\033[H\033[J"); // 清屏

    }

  }
}
