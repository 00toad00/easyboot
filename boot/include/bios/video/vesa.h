#pragma once

#include <stdint.h>

typedef struct {
    char     signature[4];
    uint16_t version;
    uint32_t oem_string;        /* far pointer */
    uint32_t capabilities;
    uint32_t mode_list;         /* far pointer */
    uint16_t total_memory;      /* × 64 KiB */
    uint8_t  rest[492];
} __attribute__((packed)) vbe_info_block;

typedef struct {
    uint16_t attributes;
    uint8_t  win_a_attributes;
    uint8_t  win_b_attributes;
    uint16_t win_granularity;
    uint16_t win_size;
    uint16_t win_a_segment;
    uint16_t win_b_segment;
    uint32_t win_func_ptr;
    uint16_t bytes_per_scan_line;
    uint16_t width;
    uint16_t height;
    uint8_t  x_char_size;
    uint8_t  y_char_size;
    uint8_t  planes;
    uint8_t  bpp;
    uint8_t  banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  image_pages;
    uint8_t  reserved0;
    uint8_t  red_mask_size;
    uint8_t  red_position;
    uint8_t  green_mask_size;
    uint8_t  green_position;
    uint8_t  blue_mask_size;
    uint8_t  blue_position;
    uint8_t  rsvd_mask_size;
    uint8_t  rsvd_position;
    uint8_t  direct_color_info;
    uint32_t framebuffer;
    uint32_t reserved1;
    uint16_t reserved2;
    uint16_t lin_bytes_per_scan_line;
    uint8_t  bnk_image_pages;
    uint8_t  lin_image_pages;
    uint8_t  lin_red_mask_size;
    uint8_t  lin_red_position;
    uint8_t  lin_green_mask_size;
    uint8_t  lin_green_position;
    uint8_t  lin_blue_mask_size;
    uint8_t  lin_blue_position;
    uint8_t  lin_rsvd_mask_size;
    uint8_t  lin_rsvd_position;
    uint32_t max_pixel_clock;
    uint8_t  reserved3[190];
} __attribute__((packed)) vbe_mode_info;

typedef struct {
    uint16_t pixel_clock;
    uint8_t  h_active_lo;
    uint8_t  h_blank_lo;
    uint8_t  h_hi;
    uint8_t  v_active_lo;
    uint8_t  v_blank_lo;
    uint8_t  v_hi;
    uint8_t  h_sync_offset_lo;
    uint8_t  h_sync_width_lo;
    uint8_t  v_sync_lo;
    uint8_t  sync_hi;
    uint8_t  h_size_mm_lo;
    uint8_t  v_size_mm_lo;
    uint8_t  size_mm_hi;
    uint8_t  h_border;
    uint8_t  v_border;
    uint8_t  flags;
} __attribute__((packed)) edid_timing;

typedef struct {
    uint16_t zero0;
    uint8_t  zero1;
    uint8_t  tag;
    uint8_t  zero2;
    uint8_t  data[13];
} __attribute__((packed)) edid_text;

typedef union {
    edid_timing timing;
    edid_text   text;
    uint8_t     raw[18];
} edid_descriptor;

typedef struct {
    uint8_t  header[8];
    uint8_t  manufacturer[2];
    uint16_t product;
    uint32_t serial;
    uint8_t  week;
    uint8_t  year;
    uint8_t  version;
    uint8_t  revision;
    uint8_t  input;
    uint8_t  width_cm;
    uint8_t  height_cm;
    uint8_t  gamma;
    uint8_t  features;
    uint8_t  chromaticity[10];
    uint8_t  established[3];
    uint16_t standard[8];
    edid_descriptor descriptors[4];
    uint8_t  extensions;
    uint8_t  checksum;
} __attribute__((packed)) edid_raw;

typedef struct {
    char     vendor[4];      
    uint16_t product;
    uint16_t year;
    uint8_t  width_cm, height_cm;
    uint16_t width, height;      
    uint16_t refresh_hz;
    char     name[14];         
} edid_info;

uint16_t get_vbe_controller_info(vbe_info_block* vbeib);
uint16_t get_vbe_mode_info(int mode, vbe_mode_info *vbemi);
uint16_t vbe_set_mode(uint16_t width, uint16_t height, uint8_t bpp, vbe_info_block *vbeib, vbe_mode_info *vbemi);

int vbe_read_edid(edid_raw *edid, uint16_t block);
int edid_parse(const edid_raw *e, edid_info *out);