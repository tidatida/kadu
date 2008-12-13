#ifndef CONFIGURATION_WINDOW_WIDGETS_H
#define CONFIGURATION_WINDOW_WIDGETS_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtXml/QDomElement>

#include "configuration/widgets/manage-accounts.h"
#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "preview.h"
#include "select_file.h"
#include "select_font.h"
#include "syntax_editor.h"

#include "configuration-window-data-manager.h"
#include "exports.h"

class ConfigGroupBox;
class ConfigurationWindow;

/**
	&lt;widget id="id" tool-tip="toolTip" /&gt;

	@arg id - id dost�pne dla aplikacji/modu��w
	@arg toolTip - podpowied� dla u�ytkownika
 **/
class KADUAPI ConfigWidget
{
	friend class ConfigurationWindow;

protected:
	ConfigGroupBox *parentConfigGroupBox;
	QString widgetCaption;
	QString toolTip;
	ConfigurationWindowDataManager *dataManager;

	virtual void createWidgets() = 0;

public:
	ConfigWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigWidget() {}

	virtual void loadConfiguration() = 0;
	virtual void saveConfiguration() = 0;

	virtual void show() = 0;
	virtual void hide() = 0;
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;widget config-section="section" config-item="item" /&gt;

	@arg section - sekcja w kt�rej trzymany jest element
	@arg item - element, w kt�rym trzymana jest warto�� parametry
 **/
class KADUAPI ConfigWidgetValue : public ConfigWidget
{
protected:
	QString section;
	QString item;

public:
	ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigWidgetValue() {}

	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;line-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigLineEdit : public QLineEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigLineEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;gg-password-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigGGPasswordEdit : public ConfigLineEdit
{
public:
	ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigGGPasswordEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

/**
	&lt;check-box caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigCheckBox : public QCheckBox, public ConfigWidgetValue
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,  ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigCheckBox() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;spin-box caption="caption" id="id" min-value="minValue" max-value="maxValue" step="step" /&gt;

	@arg minValue - minimalna warto�� (obowi�zkowo)
	@arg maxValue - maksymalna warto�� (obowi�zkowo)
	@arg step - krok warto�ci (nieobowiazkowo, domy�lnie 1)
 **/
class KADUAPI ConfigSpinBox : public QSpinBox, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigSpinBox();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;combo-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/combo-box&gt;

	@arg value - warto�� zapisana do pliku konfiguracyjnego
	@arg caption - warto�� wy�wietlana
 **/
class KADUAPI ConfigComboBox : public QComboBox, public ConfigWidgetValue
{
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigComboBox();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;hot-key-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigHotKeyEdit : public HotKeyEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,  ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigHotKeyEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;path-list-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigPathListEdit : public PathListEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigPathListEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;colob-button caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigColorButton : public ColorButton, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigColorButton();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;select-font caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigSelectFont : public SelectFont, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox0, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSelectFont();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;syntax-editro caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigSyntaxEditor : public SyntaxEditor, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSyntaxEditor();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;action-button caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigActionButton : public QPushButton, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigActionButton() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;select-file caption="caption" id="id" type="type" /&gt;

	@arg type all = wszystkie pliki, images = obrazki, audio = pliki d�wi�kowe
 **/
class KADUAPI ConfigSelectFile : public SelectFile, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSelectFile(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QString &type, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSelectFile();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;preview caption="caption" id="id" /&gt;

	Podgl�d sk�adni.
 **/
class KADUAPI ConfigPreview : public Preview, public ConfigWidget
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigPreview(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigPreview();

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;slider caption="caption" id="id" /&gt;

	@arg minValue - minimalna warto�� (obowi�zkowo)
	@arg maxValue - maksymalna warto�� (obowi�zkowo)
	@arg pageStep - krok warto�ci (obowi�zkowo)
 **/
class KADUAPI ConfigSlider : public QSlider, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name);
	ConfigSlider(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigSlider();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;label caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigLabel : public QLabel, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigLabel(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigLabel() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;list-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/list-box&gt;

	@arg value - warto�� zapisana do pliku konfiguracyjnego
	@arg caption - warto�� wy�wietlana
 **/
class KADUAPI ConfigListWidget : public QListWidget, public ConfigWidget
{
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigListWidget(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigListWidget();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue() { return itemValues[currentRow()]; }

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};


/**
	&lt;manage-accounts caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigManageAccounts : public ManageAccounts, public ConfigWidget
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigManageAccounts(const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name);
	ConfigManageAccounts(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigManageAccounts();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif // CONFIGURATION_WINDOW_WIDGETS_H
