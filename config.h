/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 2048

/*
 * function            description                     argument (example)
 *
 * battery_perc        battery percentage              battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_remaining   battery remaining HH:MM         battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_state       battery charging state          battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * cat                 read arbitrary file             path
 * cpu_freq            cpu frequency in MHz            NULL
 * cpu_perc            cpu usage in percent            NULL
 * datetime            date and time                   format string (%F %T)
 * disk_free           free disk space in GB           mountpoint path (/)
 * disk_perc           disk usage in percent           mountpoint path (/)
 * disk_total          total disk space in GB          mountpoint path (/)
 * disk_used           used disk space in GB           mountpoint path (/)
 * entropy             available entropy               NULL
 * gid                 GID of current user             NULL
 * hostname            hostname                        NULL
 * ipv4                IPv4 address                    interface name (eth0)
 * ipv6                IPv6 address                    interface name (eth0)
 * kernel_release      `uname -r`                      NULL
 * keyboard_indicators caps/num lock indicators        format string (c?n?)
 *                                                     see keyboard_indicators.c
 * keymap              layout (variant) of current     NULL
 *                     keymap
 * load_avg            load average                    NULL
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * num_files           number of files in a directory  path
 *                                                     (/home/foo/Inbox/cur)
 * ram_free            free memory in GB               NULL
 * ram_perc            memory usage in percent         NULL
 * ram_total           total memory size in GB         NULL
 * ram_used            used memory in GB               NULL
 * run_command         custom shell command            command (echo foo)
 * swap_free           free swap in GB                 NULL
 * swap_perc           swap usage in percent           NULL
 * swap_total          total swap size in GB           NULL
 * swap_used           used swap in GB                 NULL
 * temp                temperature in degree celsius   sensor file
 *                                                     (/sys/class/thermal/...)
 *                                                     NULL on OpenBSD
 *                                                     thermal zone on FreeBSD
 *                                                     (tz0, tz1, etc.)
 * uid                 UID of current user             NULL
 * up                  interface is running            interface name (eth0)
 * uptime              system uptime                   NULL
 * username            username of current user        NULL
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 *                                                     NULL on OpenBSD/FreeBSD
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 * wifi_perc           WiFi signal in percent          interface name (wlan0)
 */
static const struct arg args[] = {
      /* function              format string             argument */

    /* --- System & User Info --- */
    // { username,             "👤 %s | ",               NULL },
    // { uid,                  "UID: %s | ",            NULL },
    // { gid,                  "GID: %s | ",            NULL },
    // { hostname,             "H: %s | ",              NULL },
    // { kernel_release,       "🐧 %s | ",             NULL },
    // { uptime,               "⏳ %s | ",               NULL },
    { load_avg,             "Load: %s | ",           NULL },
    // { entropy,              "🎲 Entropy: %s | ",    NULL },

    /* --- CPU --- */
    { cpu_perc,             "💻 %s%% | ",            NULL },
    // { cpu_freq,             "⚙️ %s MHz | ",          NULL },

    /* --- Memory (RAM & Swap) --- */
    { ram_perc,             "🧠 %s%% | ",            NULL },
    // { ram_used,             "🧠 Used: %s | ",       NULL },
    // { ram_free,             "🧠 Free: %s | ",       NULL },
    // { ram_total,            "🧠 Total: %s | ",      NULL },
    { swap_perc,            "SWP: %s%% | ",          NULL },
    // { swap_used,            "SWP Used: %s | ",      NULL },
    // { swap_free,            "SWP Free: %s | ",      NULL },
    // { swap_total,           "SWP Total: %s | ",     NULL },

    /* --- Disk --- */
    { disk_perc,            "💾 /: %s%% | ",          "/" }, // Change "/" to your mountpoint if needed
    // { disk_free,            "💾 / Free: %s | ",      "/" },
    // { disk_used,            "💾 / Used: %s | ",      "/" },
    // { disk_total,           "💾 / Total: %s | ",     "/" },
    // { num_files,            "📁 %s files | ",        "/home/your_user/Documents" }, /* Customize path */

    /* --- Network --- */
    // { netspeed_rx,          "↓%s ",                 "wlan0" }, /* Customize interface */
    // { netspeed_tx,          "↑%s | ",               "wlan0" }, /* Customize interface */
    // { ipv4,                 "IPv4: %s | ",           "eth0" },  /* Customize interface */
    // { ipv6,                 "IPv6: %s | ",           "eth0" },  /* Customize interface */
    // { up,                   "eth0: %s | ",           "eth0" },  /* Customize interface */
    // { wifi_essid,           "📡 %s ",               "wlan0" }, /* Customize interface */
    { wifi_perc,            "(%s%%) | ",            "wlp6s0" }, /* Customize interface */

    /* --- Temperature --- */
    // { temp,                 "🌡️ %s°C | ",            "/sys/class/thermal/thermal_zone0/temp" }, /* Verify path */

    /* --- Audio Volume --- */
    // { vol_perc,             " %s%% | ",            "" }, /* Or "PCM", "/dev/mixer", or NULL. Check your system. */
    { run_command,             " %s%% | ",            "pamixer --get-volume" }, /* Or "PCM", "/dev/mixer", or NULL. Check your system. */

    /* --- Keyboard & Keymap --- */
    // { keymap,               "⌨️ Layout: %s | ",     NULL },
    // { keyboard_indicators,"Indic: %s | ",          "c?n?" }, /* c? for caps, n? for num. e.g., "C N" if both on */

    /* --- Battery --- */
    // { battery_state,        "%s ",                  "BAT0" }, /* Customize BAT0 if needed */
    { battery_perc,         "🔋%s%% ",              "BAT0" }, /* Customize BAT0 if needed */
    // { battery_remaining,    "(%s) | ",              "BAT0" }, /* Customize BAT0 if needed */

    /* --- Date & Time --- */
    // { datetime,             "🗓️ %s ",               "%a %b %d" }, // Example: Fri May 16
    { datetime,             "🕒 %s",                "%T" },       // Example: 21:49:50

    /* --- Custom Commands & Files (Examples, MUST be customized) --- */
    // { run_command,          "Cmd: %s | ",            "echo 'Hello DWM'" }, /* Customize command */
    // { cat,                  "File: %s | ",           "/tmp/my_value.txt" }, /* Customize file path */
};
