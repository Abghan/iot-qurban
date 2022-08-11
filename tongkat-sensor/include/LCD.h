#ifndef lcd_H
#define lcd_H
#include <Arduino.h>

#define SH1106

enum md_button { btn_idle = 0, btn_hold, btn_send, btn_clear };
enum md_device { md_thermoscan = 0, md_rfidreader, md_thermo_rfid, md_timbangan, md_timbangan_rfid, md_allin};

struct tbl_oled
{
    int signal = 0;
    int8_t mode_device = 0; //0 = SUHU, 1 = BERAT, 2 = RFID
    int8_t mode_button = 0; //0 = idle, 1 = hold, 2 = send, 3 = clear
    bool on_send = false; 
    String header = "";
    String text = "";
    int size = 1;
};


class lcd
{
private:
    void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h);
    void testdrawchar(void);
    void testdrawcircle(void);
    void testfillrect(void);
    void testdrawtriangle(void);
    void testfilltriangle(void);
    void testdrawroundrect(void);
    void testfillroundrect(void);
    void testdrawrect(void);
    void testdrawline();
    void testscrolltext(void);
    void disp_idle();

    void disp_bar();
    int getBarsSignal(long rssi);
public:
    bool onsend_data = false;
    bool btn_rfid = false;
    bool btn_suhu = false;
    bool on_draw_notif = false;

    bool mqtt_status = false;
    bool thermo_on_hold = false;
    bool rfid_on_hold = false;
    int8_t mode_hardware = md_thermo_rfid;

    bool wifi_is_connected = false;
    // size = 1, maksimum 21 karaker
    // size = 2, maksimum 10 karaker
    // size = 3, maksimum 7 karaker
    // size = 4, maksimum 5 karaker
    // size = 5, maksimum 4 karaker
#ifdef SH1106
    void draw();
    long val_rssi;
    bool temperature_ok = false;
    bool rfid_ok = false;

    String val_suhu = "";
    String val_rfid = "";

    struct tbl_oled form;
    String OLED_Device;
    bool idle = true;
    void setup();
    void loop();

    void clear_oled();
    void draw_rfid(String rfid);
    void draw_suhu(String suhu);
    void draw_idle();
    void draw_notif(String header, String baris1, String baris2);

#endif
};
#endif