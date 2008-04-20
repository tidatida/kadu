#ifndef KADU_CONFIG_WIZARD_H
#define KADU_CONFIG_WIZARD_H

#include <QDialog>
#include <QStackedWidget>
#include <QGridLayout>
#include <QList>

#include "../account_management/register.h"

#include "chat_widget.h"
#include "misc.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QRadioButton;

/**
 * @defgroup config_wizard Config wizard
 * @{
 */
class Wizard : public QDialog
{
    Q_OBJECT

	QStackedWidget *pageArea;
	QGridLayout *layout;
	QPushButton *nextButton;
	QPushButton *backButton;
	QPushButton *cancelButton;
	QPushButton *finishButton;

	QPushButton *ggRegisterAccount;

	QRadioButton *haveNumber;
	QLineEdit *ggNumber;
	QLineEdit *ggPassword;
	QCheckBox *ggImportContacts;
	QRadioButton *dontHaveNumber;
	QLineEdit *ggNewPassword;
	QLineEdit *ggReNewPassword;
	QLineEdit *ggEMail;

	QList<QWidget *> haveNumberWidgets;
	QList<QWidget *> dontHaveNumberWidgets;

	QComboBox *browserCombo;
	QLineEdit *browserCommandLineEdit;
	QComboBox *mailCombo;
	QLineEdit *mailCommandLineEdit;

	QComboBox *soundModuleCombo;
	QPushButton *soundTest;

	QString backupSoundModule;

	void createGGAccountPage();
	void createApplicationsPage();
	void createSoundPage();

	void loadGGAccountOptions();
	void loadApplicationsOptions();
	void loadSoundOptions();

	void saveGGAccountOptions();
	void saveApplicationsOptions();
	void saveSoundOptions();

	void tryImport();

	void changeSoundModule(const QString &newModule);

private slots:
	void haveNumberChanged(bool haveNumber);
	void registerGGAccount();
	void registeredGGAccount(bool ok, UinType uin);

	void finishClicked();
	void cancelClicked();
	void nextClicked();
	void backClicked();

	void browserChanged(int index);
	void emailChanged(int index);

	void testSound();

protected:
	void closeEvent(QCloseEvent *e);

public:
	Wizard(QWidget *parent = 0, const char *name = 0, bool modal = false);
	~Wizard();

	void addPage(QWidget *page, const QString &title, const QString &description, bool lastOne);

public slots:
	void wizardStart();

};

class WizardStarter : public QObject
{
	Q_OBJECT

	int menuPos;

public:
	WizardStarter(QObject *parent=0, const char *name=0);
	~WizardStarter();

public slots:
	void start();

	void userListImported(bool ok, QList<UserListElement> list);
	void connected();

};

extern WizardStarter *wizardStarter;
extern Wizard *startWizardObj;

/** @} */

#endif
