/* Tymczasowy plik potrzebny do kompilacji na windowsa, potrzebne odkomentowac */
extern "C" int docking_init(bool);
extern "C" void docking_close(void);
extern "C" int qt4_docking_init(bool);
extern "C" void qt4_docking_close(void);
//extern "C" int account_management_init(bool);
//extern "C" void account_management_close(void);
//extern "C" int autoaway_init(bool);
//extern "C" void autoaway_close(void);
//extern "C" int dcc_init(bool);
//extern "C" void dcc_close(void);
//extern "C" int default_sms_init(bool);
//extern "C" void default_sms_close(void);
//extern "C" int hints_init(bool);
//extern "C" void hints_close(void);
//extern "C" int history_init(bool);
//extern "C" void history_close(void);
//extern "C" int notify_init(bool);
//extern "C" void notify_close(void);
//extern "C" int sms_init(bool);
//extern "C" void sms_close(void);
//extern "C" int window_notify_init(bool);
//extern "C" void window_notify_close(void);
//extern "C" int config_wizard_init(bool);
//extern "C" void config_wizard_close(void)
//extern "C" int advanced_userlist_init(bool);
//extern "C" void advanced_userlist_close(void)
//extern "C" int autoresponder_init(bool);
//extern "C" void autoresponder_close(void)
void ModulesManager::registerStaticModules()
{
	registerStaticModule("docking", docking_init, docking_close);
	registerStaticModule("qt4_docking", qt4_docking_init, qt4_docking_close);
//	registerStaticModule("account_management", account_management_init, account_management_close);
//	registerStaticModule("autoaway", autoaway_init, autoaway_close);
//	registerStaticModule("dcc", dcc_init, dcc_close);
//	registerStaticModule("default_sms", default_sms_init, default_sms_close);
//	registerStaticModule("hints", hints_init, hints_close);
//	registerStaticModule("history", history_init, history_close);
//	registerStaticModule("notify", notify_init, notify_close);
//	registerStaticModule("sms", sms_init, sms_close);
//	registerStaticModule("window_notify", window_notify_init, window_notify_close);
//	registerStaticModule("config_wizard", config_wizard_init, config_wizard_close);
//	registerStaticModule("advanced_userlist", advanced_userlist_init, advanced_userlist_close);
//	registerStaticModule("autoresponder", autoresponder_init, autoaway_close);
}
