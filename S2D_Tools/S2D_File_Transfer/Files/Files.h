#ifndef APP_S2D_TOOLS_FILES_H
#define APP_S2D_TOOLS_FILES_H

typedef struct S2D_Data_Packet s2d_data_packet_s;

void s2d_init_file(TaskHandle_t* rt_handle);
void s2d_init_packet(s2d_data_packet_s* packet);
void s2d_serialize_packet(const s2d_data_packet_s* packet, uint8_t* buffer);
void s2d_deserialize_header(s2d_data_packet_s *packet, uint8_t* buffer);


#endif //APP_S2D_TOOLS_FILES_H
