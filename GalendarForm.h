/*
 * GalendarForm.h
 *
 *  Created on: Nov 20, 2011
 *      Author: vincent
 */

#ifndef GALENDARFORM_H_
#define GALENDARFORM_H_

#include "ui_GalendarForm.h"
#include <QDateTime>
#include <QMap>
#include <QScriptValue>
#include <QScriptEngine>

class QNetworkAccessManager;
class QNetworkReply;

class GalendarForm: public QWidget {
Q_OBJECT
public:
	static QString addQuery(const QString &originalParams, const QString &name,
			const QString &value);

public:
	GalendarForm(QWidget *parent = 0);

public slots:
	void on_authorizeButton_clicked();
	void on_webView_urlChanged(const QUrl &url);
	void on_urlEdit_returnPressed();
	void on_clearLogButton_clicked();
	void on_webView_loadFinished(bool ok);
	void on_webView_loadStarted();
//	void on_requestAccessTokenButton_clicked();
	void on_testGoButton_clicked();
	void readReply(QNetworkReply *reply);
	void readAccessToken(QNetworkReply *reply);
	void readApiReaderReply(QNetworkReply *reply);
	void readApiWriterReply(QNetworkReply *reply);
	void requestAccessToken();
//	void on_testWebView_urlChanged(const QUrl &url);
	void on_clientIdEdit_textChanged(const QString &clientId);
	void on_clientSecretEdit_textChanged(const QString &clientSecret);
	void on_scopeEdit_textChanged(const QString &scope);
	void on_debugModeCheck_toggled(bool checked);
	void on_calListButton_clicked();
	void on_calCombo_currentIndexChanged(const QString &calId);
	void getAllCalendars();
	void getAllEvents();
	void on_eventCheckButton_clicked();
	void on_calCheckButton_clicked();
	void on_eventCreateButton_clicked();
	void on_projectAddButton_clicked();
	void on_projectDeleteButton_clicked();
	void on_eventAddButton_clicked();
	void on_eventDeleteButton_clicked();
	void selectedProjectChanged();
//	void selectedEventChanged();
	void selectEvents();
	void selectProjects();
	void on_publishButton_clicked();
	void on_calendarNewButton_clicked();
	void on_clearDatabaseButton_clicked();
	void on_eventListButton_clicked();
	void syncProjectSelection(const QModelIndex &currentIndex);
	void contentModified();
	void on_projectSaveButton_clicked();
//	void on_formatAttendeeButton_clicked();
	void on_publishExistingButton_clicked();
	void on_aclListButton_clicked();
	void createAcl(const QString &calId, const QString &attendees,
			const QString &role = "writer");
	void on_editEventsButton_clicked();
	void on_backToOverviewButton_clicked();
	void on_calmanDeleteButton_clicked();
	void selectCalman();
	void on_addRowButton_clicked();
	void on_addColButton_clicked();

protected:
	virtual void closeEvent(QCloseEvent *event);
	bool eventFilter(QObject *object, QEvent *event);

private:
	static QString addNewCalendarString;
	static QString defaultTimeZone;
	Ui::GalendarForm ui;
	QNetworkAccessManager *accTokenNetMan;
	QNetworkAccessManager *apiReaderNetMan;
	QNetworkAccessManager *apiWriterNetMan;
	QDateTime tokenExpiration;
	bool debugMode;
	QString oaAuthUrl;
	QString oaAccUrl;
	QString authToken;
	QString accToken;
	QString refreshToken;
	QString accType;
	QString oaClientId;
	QString oaClientSecret;
	QString oaScope;
	QString calApiBaseUrl;
	QScriptValue calendars;
	QScriptValue events;
	QScriptEngine scriptEngine;
	QString userEmail;
	QString dbName;
	int currentProjectId;
//	int currentEventId;
	bool projectAttributesEdited;
	QString newCalId;
	int requestCount;

private:
	static QString getTimestamp();
	static QString getNewEventPost(const QString &eventName,
			const QString &startDateTime, const QString &endDateTime,
			bool allday = false, const QString &description = QString(),
			const QString &location = QString(), const QStringList &attendees =
					QStringList(), const QString &timeZone = defaultTimeZone);
	static QString getNewCalendarPost(const QString &calendarName,
			const QString &timeZone = defaultTimeZone);
	static QString getNewAclPost(const QString &attendee, const QString &role =
			"writer");
//	static QString getUtcOffset();
	void createEvent(const QString &calId, const QString &eventName,
			const QString &startDateTime, const QString &endDateTime,
			bool allday = false, const QString &description = QString(),
			const QString &location = QString(), const QStringList &attendees =
					QStringList(), const QString &timeZone = defaultTimeZone);
	void createCalendar(const QString &calendarName, const QString &timeZone =
			defaultTimeZone);
	QString getCurrentCalendarId() const;
	QString getCurrentComboEventId() const;
	QString getCalendarTimeZone(const QString &calId) const;
	void init();
	void loadSettings();
	void saveSettings();
	void makeConnections();
	void log(const QString &message);
	void setDebugWidgetsVisible(bool visiable);
//	void updateCalCombo(const QStringList &calNames);
//	void updateEventCombo(const QStringList &eventNames);
	void getAllEventsFromCalendar(const QString &calId);
	void getAclFromCalendar(const QString &calId);
	bool prepareDB();
	QScriptValue getCalendarById(const QString &calId) const;
	void publishCalendarId(const QString &calId);
	QString getFormattedAttendees(const QString &attendees);
	int getCurrentEventId() const;
};

#endif /* GALENDARFORM_H_ */
