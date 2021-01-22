struct menu_t;

typedef void (*menu_cb_t)(const struct menu_t *menu);

struct menu_t {
	const icon_15x15_t *icon;
	const struct menu_t *entries;
	const struct menu_t *next;
	menu_cb_t cb;
	menu_cb_t enter;
	menu_cb_t leave;
};

typedef struct menu_t menu_t;

extern const menu_t menu_back;
extern const menu_t menu_app;
extern const menu_t menu_settings;
extern const menu_t menu_brightness;
