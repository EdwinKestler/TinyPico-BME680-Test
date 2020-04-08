//coloque en este archivo declaración de tipo de datos compuestos que define una lista de variables agrupadas 
//físicamente con un solo nombre en un bloque de memoria

const char *NTP_Server = "0.north-america.pool.ntp.org";

struct WConfig
{
    /* Wifi data */
    char SSID[12] = "HITRON-2F50";
    char password[13] = "PSZ2ZFT2BF77";
};

struct NTPConfig
{
    /* NTP data */
    int NTP_port = 3600;
    int NTP_interval= 6000;
};
