#ifndef DCC_H
#define DCC_H

#include <qdialog.h>
#include <qlayout.h> 
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsocketnotifier.h>
#include <qhostaddress.h>

#include "kadu-config.h"
#include "libgadu.h"

extern QSocketNotifier* dccsnr;
extern QSocketNotifier* dccsnw;
extern QHostAddress config_dccip;
extern QHostAddress config_extip;
extern struct gg_dcc* dccsock;

enum DccFileDialogType {
	DCC_TYPE_SEND,
	DCC_TYPE_GET
};

enum DccType {
	DCC_TYPE_FILE,
	DCC_TYPE_VOICE
};

class dccSocketClass;

class DccFileDialog : public QDialog {
	Q_OBJECT
	public:
		DccFileDialog(dccSocketClass *dccsocket, int type = DCC_TYPE_GET,
			QDialog *parent = 0, const char *name = 0);
		~DccFileDialog();
		void printFileInfo(struct gg_dcc *dccsock);
		void updateFileInfo(struct gg_dcc *dccsock);

		bool dccFinished;				
	protected:
		void closeEvent(QCloseEvent *e);

		dccSocketClass *dccsocket;
		QLabel *l_offset;
		QProgressBar *p_progress;
		QVBoxLayout* vlayout1;
		long long int prevPercent;
		QTime *time;
		int prevOffset;
		int type;
};

enum dccSocketState {
	DCC_SOCKET_TRANSFERRING,
	DCC_SOCKET_CONNECTION_BROKEN,
	DCC_SOCKET_TRANSFER_ERROR,
	DCC_SOCKET_TRANSFER_FINISHED,
	DCC_SOCKET_COULDNT_OPEN_FILE,
	DCC_SOCKET_TRANSFER_DISCARDED,
	DCC_SOCKET_VOICECHAT_DISCARDED
};

class dccSocketClass : public QObject {
	Q_OBJECT
	public:
		dccSocketClass(struct gg_dcc *dcc_sock, int type = DCC_TYPE_FILE);
		~dccSocketClass();
		virtual void initializeNotifiers();
		virtual void watchDcc(int check);

		int type;
		int state;
		static int count;

	public slots:
		void setState(int pstate);

	signals:
		void dccFinished(dccSocketClass *dcc);

	protected:
		virtual void connectionBroken();
		virtual void dccError();
		virtual void dccEvent();
		void askAccept();
		QString selectFile();

		QSocketNotifier *snr;
		QSocketNotifier *snw;
		struct gg_dcc *dccsock;
		struct gg_event *dccevent;
		DccFileDialog *filedialog;
		bool in_watchDcc;

	protected slots:
		void dccDataReceived();
		void dccDataSent();
};

#endif
