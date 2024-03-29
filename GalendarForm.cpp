/*
 * GalendarForm.cpp
 *
 *  Created on: Nov 20, 2011
 *      Author: vincent
 */

#include "GalendarForm.h"
#include <QtGui>
#include <QtNetwork>
#include <QtScript>
#include <QtSql>
#include <iostream>
#include "EventDelegate.h"
#include "CalmanDelegate.h"

QString GalendarForm::addNewCalendarString = QString("+ New Calendar");
QString GalendarForm::defaultTimeZone;

GalendarForm::GalendarForm(QWidget *parent) :
		QWidget(parent) {
	ui.setupUi(this);
	init();
	makeConnections();
	loadSettings();
}

void GalendarForm::on_authorizeButton_clicked() {
	QString requestParam = "response_type=code";
	requestParam += "&client_id=" + oaClientId;
	requestParam += "&redirect_uri=urn:ietf:wg:oauth:2.0:oob";
	requestParam += "&scope=" + oaScope;
	QString requestUrl = oaAuthUrl + "?" + requestParam;
//	log(requestUrl);
	ui.webView->load(QNetworkRequest(QUrl(requestUrl)));
	ui.webView->setFocus();
}

void GalendarForm::on_webView_urlChanged(const QUrl &url) {
//	if (url.toString().startsWith(
//			"https://accounts.google.com/o/oauth2/approval")) {
//		ui.webView->stop();
//		log(url.toString());
//		netMan->get(QNetworkRequest(QUrl(url)));
//	} else {
	ui.urlEdit->setText(url.toString());
//	}
	log("[URL CHANGE] " + url.toString());
}

void GalendarForm::on_urlEdit_returnPressed() {
	ui.webView->load(QNetworkRequest(QUrl(ui.urlEdit->text())));
}

void GalendarForm::init() {
	oaAuthUrl = "https://accounts.google.com/o/oauth2/auth";
	oaAccUrl = "https://accounts.google.com/o/oauth2/token";
	accTokenNetMan = new QNetworkAccessManager(this);
	apiReaderNetMan = new QNetworkAccessManager(this);
	apiWriterNetMan = new QNetworkAccessManager(this);
	oaClientId = "1003377928301.apps.googleusercontent.com";
	oaClientSecret = "_CPiLDKpv117r4JS0TNGJx71";
	oaScope = "https://www.googleapis.com/auth/userinfo.email "
			"https://www.googleapis.com/auth/calendar "
	/*"http://www-opensocial.googleusercontent.com/api/people"*/;
	calApiBaseUrl = "https://www.googleapis.com/calendar/v3";
	currentProjectId = -1;
//	currentEventId = -1;
	requestCount = 0;
	projectAttributesEdited = false;
	QString storePath = QDesktopServices::storageLocation(
			QDesktopServices::DataLocation);
	QDir().mkpath(storePath);
	dbName = storePath + QDir::separator() + "galendar.db";
//	calendars.clear();
//	events.clear();
	ui.clientIdEdit->setText(oaClientId);
	ui.clientSecretEdit->setText(oaClientSecret);
	ui.scopeEdit->setText(oaScope);
	ui.progressBar->hide();
	ui.logBrowser->setFont(QFont("Monospace"));
	debugMode = false;
	ui.debugModeCheck->setChecked(debugMode);
//	if (!debugMode) {
//		ui.debugModeCheck->setVisible(false);
//	}
//	ui.afterAuthWidget->setEnabled(false);
//	ui.afterCalListWidget->setEnabled(false);
//	ui.eventTable->setItemDelegate(new EventDelegate(this));
//	ui.projectView->setItemDelegate(new ProjectDelegate(this));
	ui.publishDateEdit->setDate(QDate::currentDate());
//	ui.formatAttendeeButton->setVisible(false);
//	ui.projectSaveButton->setEnabled(false);
	ui.projectNoteEdit->installEventFilter(this);
	ui.projectAttendeeEdit->installEventFilter(this);
	setDebugWidgetsVisible(debugMode);
	prepareDB();
}

void GalendarForm::makeConnections() {
	connect(ui.webView->page()->networkAccessManager(),
			SIGNAL(finished(QNetworkReply *)),
			SLOT(readReply(QNetworkReply *)));
	connect(ui.webView, SIGNAL(loadProgress(int)), ui.progressBar,
			SLOT(setValue(int)));
	connect(ui.testWebView, SIGNAL(loadProgress(int)), ui.testProgressBar,
			SLOT(setValue(int)));
	connect(accTokenNetMan, SIGNAL(finished(QNetworkReply *)),
			SLOT(readAccessToken(QNetworkReply *)));
	connect(apiReaderNetMan, SIGNAL(finished(QNetworkReply *)),
			SLOT(readApiReaderReply(QNetworkReply *)));
	connect(apiWriterNetMan, SIGNAL(finished(QNetworkReply *)),
			SLOT(readApiWriterReply(QNetworkReply *)));
	connect(
			ui.projectView->selectionModel(),
			SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
			this, SLOT(syncProjectSelection(const QModelIndex &)));
	connect(ui.projectNoteEdit, SIGNAL(textChanged()), SLOT(contentModified()));
	connect(ui.projectAttendeeEdit, SIGNAL(textChanged()),
			SLOT(contentModified()));
}

void GalendarForm::readReply(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
//	log(bytes);
	log(tr("[REQUEST] %1").arg(reply->request().url().toString()));
//	foreach(QByteArray header, reply->request().rawHeaderList()) {
//		log("[HEADER] " + header + "==>" + reply->request().rawHeader(header));
//	}
	reply->deleteLater();
}

void GalendarForm::log(const QString &message) {
	ui.logBrowser->append(tr("[%1] %2").arg(getTimestamp()).arg(message));
}

void GalendarForm::on_clearLogButton_clicked() {
	ui.logBrowser->clear();
}

void GalendarForm::on_webView_loadFinished(bool ok) {
	ui.progressBar->hide();
	if (ok) {
		QUrl url = ui.webView->url();
		if (url.toString().startsWith(
				"https://accounts.google.com/o/oauth2/approval")) {
			authToken = ui.webView->title().split("=")[1];
//			log(url.toString());
			log("[AUTH TOKEN] " + authToken);
//			ui.webView->load(QNetworkRequest(QUrl("about:blank")));
			ui.webView->setContent(tr("Please wait...").toUtf8(), "text/plain");
			requestAccessToken();
//			ui.webView->setHtml();
		}
	}
}

void GalendarForm::on_webView_loadStarted() {
	ui.progressBar->show();
}

void GalendarForm::requestAccessToken() {
	QString requestParam = "grant_type=authorization_code";
	requestParam += "&client_id=" + oaClientId;
	requestParam += "&redirect_uri=urn:ietf:wg:oauth:2.0:oob";
	requestParam += "&client_secret=" + oaClientSecret;
	requestParam += "&code=" + authToken;
	accTokenNetMan->post(QNetworkRequest(QUrl(oaAccUrl)),
			requestParam.toUtf8());
	//		QString requestParam = "redirect_uri=urn:ietf:wg:oauth:2.0:oob";
	//		requestParam += "&client_id=" + ui.clientIdEdit->text();
	//		requestParam += "&grant_type=authorization_code";
	//		requestParam += "&client_secret=" + ui.clientSecretEdit->text();
	//		requestParam += "&code=" + authToken;
	////	QString requestUrl = oauthAccessUrl + "?" + requestParam;
	////	log(requestUrl);
	//		QNetworkRequest req = QNetworkRequest(QUrl(oauthAccessUrl));
	////		req.setHeader(QNetworkRequest::ContentTypeHeader,
	////				"application/x-www-form-urlencoded;charset=UTF-8");
	//		log(oauthAccessUrl + "?" + requestParam);
	////		accTokenNetMan->post(req, requestParam.toUtf8());
//	QString requestUrl = oauthAccessUrl + "?" + requestParam;
//	log(requestUrl);
	log(tr("[POST URL] %1").arg(oaAccUrl));
	log(tr("[POST PARAM] %1").arg(requestParam));
//	ui.webView->load(QNetworkRequest(QUrl(oauthAccessUrl)),
//			QNetworkAccessManager::PostOperation, requestParam.toUtf8());
}

//void GalendarForm::on_requestAccessTokenButton_clicked() {
//	if (!authToken.isEmpty()) {
//		requestAccessToken();
//	} else {
//		log("No auth token available");
//	}
//}

QString GalendarForm::getTimestamp() {
	return QDateTime::currentDateTime().toString("yyyyMMdd hhmmss");
}

void GalendarForm::readAccessToken(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
//	ui.testWebView->setContent(bytes);
//	log(bytes);

	QScriptValue scriptValue;
	scriptValue = scriptEngine.evaluate("(" + QString(bytes) + ")");
	log(tr("[SCRIPT PARSE] %1").arg(scriptValue.toString()));
//	} else{
//		scriptValue.toString();
//	}
	QString requestUrl = reply->request().url().toString();
	if (requestUrl.startsWith(
			"https://www.googleapis.com/oauth2/v1/userinfo")) { // Returns user's email
		userEmail = scriptValue.property("email").toString();
//		ui.webView->setContent(
//				tr("Access token for %1 acquired!").arg(userEmail).toUtf8(),
//				"text/plain");
		ui.calListButton->click();
	} else {
		accToken = scriptValue.property("access_token").toString();
		accType = scriptValue.property("token_type").toString();
		refreshToken = scriptValue.property("refresh_token").toString();
		tokenExpiration = QDateTime::fromTime_t(
				QDateTime::currentDateTime().toTime_t()
						+ scriptValue.property("expires_in").toInteger());
		log(tr("[ACCESS TOKEN] %1").arg(accToken));
		log(tr("[ACCESS TYPE] %1").arg(accType));
		log(tr("[REFRESH TOKEN] %1").arg(refreshToken));
		log(
				tr("[TOKEN EXPIRY] %1").arg(
						tokenExpiration.toString("yyyyMMdd hhmmss")));
		if (!accToken.isEmpty()) {
			QNetworkRequest req = QNetworkRequest(
					QUrl("https://www.googleapis.com/oauth2/v1/userinfo"));
			req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
			accTokenNetMan->get(req);
//			ui.afterAuthWidget->setEnabled(true);
		} else {
			ui.webView->setContent(
					tr("Failed to retrieve an access token!").toUtf8(),
					"text/plain");
		}
	}
	reply->deleteLater();
}

void GalendarForm::on_testGoButton_clicked() {
//	accTokenNetMan->post(QNetworkRequest(QUrl(ui.testUrlEdit->text())),
//			ui.testPostEdit->text().toUtf8());
	ui.testWebView->load(QNetworkRequest(QUrl(ui.testUrlEdit->text())),
			QNetworkAccessManager::PostOperation,
			ui.testPostEdit->text().toUtf8());
}

//void GalendarForm::on_testWebView_urlChanged(const QUrl &url) {
////	ui.testUrlEdit->setText(url.toString());
//}

void GalendarForm::setDebugWidgetsVisible(bool visible) {
	ui.debugWidget->setVisible(visible);
//	ui.calCheckButton->setVisible(visible);
//	ui.eventCheckButton->setVisible(visible);
//	ui.requestAccessTokenButton->setVisible(visible);
	ui.urlEdit->setVisible(visible);
	if (visible) {
		ui.tabWidget->addTab(ui.logTab, tr("Log"));
		ui.tabWidget->addTab(ui.testTab, tr("Test"));
		ui.tabWidget->addTab(ui.scriptTab, tr("Script"));
//		ui.tabWidget->addTab(ui.pubTab, tr("Publication"));
//		ui.tabWidget->addTab(ui.contactsTab, tr("Contacts test"));
	} else {
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.logTab));
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.testTab));
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.scriptTab));
//		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.pubTab));
//		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.contactsTab));
	}
}

void GalendarForm::on_clientIdEdit_textChanged(const QString &clientId) {
	oaClientId = clientId;
}

void GalendarForm::on_clientSecretEdit_textChanged(
		const QString &clientSecret) {
	oaClientSecret = clientSecret;
}

void GalendarForm::on_scopeEdit_textChanged(const QString &scope) {
	oaScope = scope;
}

void GalendarForm::on_debugModeCheck_toggled(bool checked) {
	debugMode = checked;
	setDebugWidgetsVisible(debugMode);
}

void GalendarForm::on_calListButton_clicked() {
	if (accToken.isEmpty()) {
		QString errMsg = tr("No access token available!");
		log(tr("[ERROR] %1").arg(errMsg));
		ui.webView->setContent(errMsg.toUtf8(), "text/plain");
	} else {
		getAllCalendars();
	}
}

void GalendarForm::getAllCalendars() {
	QString calListUrl = calApiBaseUrl + "/users/me/calendarList?access_token="
			+ accToken;
	QNetworkRequest req = QNetworkRequest(QUrl(calListUrl));
//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
	log(calListUrl);
	apiReaderNetMan->get(req);
}

void GalendarForm::readApiReaderReply(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
//	--requestCount;
	if (debugMode) {
		ui.webView->setContent(bytes, "text/plain");
//	} else {
//		ui.webView->setContent(bytes, "text/plain");
	}

//	} else{
//		scriptValue.toString();
//	}
//	accToken = scriptValue.property("access_token").toString();
//	accType = scriptValue.property("token_type").toString();
//	refreshToken = scriptValue.property("refresh_token").toString();
//	tokenExpiration = QDateTime::fromTime_t(
//			QDateTime::currentDateTime().toTime_t()
//					+ scriptValue.property("expires_in").toInteger());
//	log(tr("[ACCESS TOKEN] %1").arg(accToken));
//	log(tr("[ACCESS TYPE] %1").arg(accType));
//	log(tr("[REFRESH TOKEN] %1").arg(refreshToken));
//	log(
//			tr("[TOKEN EXPIRY] %1").arg(
//					tokenExpiration.toString("yyyyMMdd hhmmss")));
	QString requestUrl = reply->request().url().toString();
	if (requestUrl.contains("/users/me/calendarList")) { // Calendar list request
		calendars = scriptEngine.evaluate("(" + QString(bytes) + ")");
		log(tr("[SCRIPT PARSE] %1").arg(calendars.toString()));
		QScriptValueIterator it(calendars.property("items"));
		ui.calCombo->clear();
		log(tr("Calendars: (+ownership; -readable)"));
//			calendars.clear();
		while (it.hasNext()) {
			it.next();
			QString accessRole = it.value().property("accessRole").toString();
			QString calId = it.value().property("id").toString();
			QString calName = it.value().property("summary").toString();
			if (/*accessRole == "writer" ||*/accessRole == "owner") {
				ui.calCombo->addItem(calName, calId);
				log("+ " + calId + " --> " + calName);
				if (calId.compare(userEmail) == 0) {
					defaultTimeZone =
							it.value().property("timeZone").toString();
					log("Primary time zone: " + defaultTimeZone);
				}
			} else {
				log("- " + calId + " --> " + calName);
			}
		}
//		ui.calCombo->adjustSize();
		if (!newCalId.isEmpty()) {
			int idx = ui.calCombo->findData(newCalId, Qt::UserRole);
			ui.calCombo->setCurrentIndex(idx < 0 ? 0 : idx);
			newCalId = "";
		}
		ui.calCombo->addItem(addNewCalendarString);
//		ui.afterCalListWidget->setEnabled(true);
		if (!debugMode) {
			ui.webView->setContent(tr("Ready to proceed").toUtf8(),
					"text/plain");
		}
	} else if (requestUrl.contains("/calendars/")
			&& requestUrl.contains("/events")) { // Event list request
		events = scriptEngine.evaluate("(" + QString(bytes) + ")");
		log(tr("[SCRIPT PARSE] %1").arg(events.toString()));
		QScriptValueIterator it(events.property("items"));
		ui.eventCombo->clear();
		log(tr("Events:"));
		while (it.hasNext()) {
			it.next();
			QString eventId = it.value().property("id").toString();
			QString eventName = it.value().property("summary").toString();
			ui.eventCombo->addItem(eventName, eventId);
			log("* " + eventId + " --> " + eventName);
		}
//		ui.afterCalListWidget->setEnabled(true);
	}
	reply->deleteLater();
}

void GalendarForm::readApiWriterReply(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
	--requestCount;
	if (debugMode) {
		ui.webView->setContent(bytes, "text/plain");
	}
	QScriptValue result = scriptEngine.evaluate("(" + QString(bytes) + ")");
	QString kind = result.property("kind").toString();
	if (kind.compare("calendar#event") == 0) {
		QString id = result.property("id").toString();
		if (id.isEmpty()) {
			log(tr("[ERROR] Event creation failed"));
			if (!debugMode) {
				ui.webView->setContent(tr("ERROR! Event creation failed. "
						"Please check network connection first.").toUtf8(),
						"text/plain");
			}
		} else {
			log(tr("[INFO] Event created with id %1").arg(id));
			if (!debugMode) {
				if (requestCount == 0) {
					ui.webView->setContent(tr("Project published.").toUtf8(),
							"text/plain");
				} else {
					ui.webView->setContent(
							tr("Waiting for %1 job(s) to finish...").arg(
									requestCount).toUtf8(), "text/plain");
				}
			}
		}
	} else if (kind.compare("calendar#calendar") == 0) {
		QString id = result.property("id").toString();
		if (id.isEmpty()) {
			log(tr("[ERROR] Calendar creation failed"));
			if (!debugMode) {
				ui.webView->setContent(tr("ERROR! Calendar creation failed. "
						"Please check network connection first.").toUtf8(),
						"text/plain");
			}
		} else {
			log(tr("[INFO] Calendar created with id %1").arg(id));
			if (!debugMode) {
				ui.webView->setContent(tr("Calendar created.").toUtf8(),
						"text/plain");
			}
			QSqlQuery query;
			query.prepare("INSERT INTO Calman (id, name, account, creation) "
					"VALUES (:id, :name, :account, :creation)");
			query.bindValue(":id", id);
			query.bindValue(":name", result.property("summary").toString());
			query.bindValue(":account", userEmail);
			query.bindValue(":creation",
					QDateTime::currentDateTime().toTime_t());
			if (!query.exec()) {
				QMessageBox::critical(0,
						qApp->tr("Calman insert statement failed"),
						qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
			} else {
				selectCalman();
			}
			newCalId = id;
			QTimer::singleShot(500, this, SLOT(getAllCalendars()));
		}
	} else if (kind.compare("calendar#aclRule") == 0) {
		QString id = result.property("id").toString();
		if (id.isEmpty()) {
			log(tr("[ERROR] ACL creation failed"));
			if (!debugMode) {
				ui.webView->setContent(
						tr("ERROR! ACL creation failed.").toUtf8(),
						"text/plain");
			}
		} else {
			log(tr("[INFO] ACL created with id %1").arg(id));
//			if (!debugMode) {
//				ui.webView->setContent(tr("ACL created.").toUtf8(),
//						"text/plain");
//			}
			if (!debugMode) {
				if (requestCount == 0) {
					ui.webView->setContent(tr("Project published.").toUtf8(),
							"text/plain");
				} else {
					ui.webView->setContent(
							tr("Waiting for %1 job(s) to finish...").arg(
									requestCount).toUtf8(), "text/plain");
				}
			}
		}
	} else {
		log(tr("[WARNING] Unexpected reply recevied."));
		log(bytes);
	}
	reply->deleteLater();
}

void GalendarForm::on_calCombo_currentIndexChanged(const QString & calId) {
	log(tr("[DEBUG] on_calCombo_currentIndexChanged(%1)").arg(calId));
	if (calId.compare(addNewCalendarString) == 0) {
		ui.calCombo->clear();
		ui.calCombo->addItem(tr("Populating calendars..."));
		ui.calendarNewButton->click();
	}
}

void GalendarForm::getAllEvents() {
	getAllEventsFromCalendar(getCurrentCalendarId());
}

void GalendarForm::getAllEventsFromCalendar(const QString &calId) {
	QString eventListUrl = calApiBaseUrl + "/calendars/" + calId + "/events"
			"?access_token=" + accToken;
	log(eventListUrl);
	apiReaderNetMan->get(QNetworkRequest(QUrl(eventListUrl)));
}

void GalendarForm::on_calCheckButton_clicked() {
	log(tr("[SCRIPT CAL] %1").arg(calendars.toString()));
}

void GalendarForm::on_eventCheckButton_clicked() {
	log(tr("[SCRIPT EVT] %1").arg(events.toString()));
}

void GalendarForm::on_eventCreateButton_clicked() {
	static int count = 0;
	if (accToken.isEmpty()) {
		log("[ERROR] No access token, no event created.");
		return;
	}
	++count;
	if (count > 9) {
		log("[INFO] Count exceeds max, no event created.");
		return;
	}
	QScriptValueIterator it(calendars.property("items"));
	QString timeZone = QString();
	while (it.hasNext()) {
		it.next();
		QString calId = it.value().property("id").toString();
		if (calId == getCurrentCalendarId()) {
			timeZone = it.value().property("timeZone").toString();
		}
	}
	QString eventName = tr("testing event %1").arg(count);
	QString start = QString("2011-12-0%1T10:00:00.000").arg(count);
	QString end = QString("2011-12-0%1T10:30:00.000").arg(count);
	QString requestUrl = calApiBaseUrl + "/calendars/" + getCurrentCalendarId()
			+ "/events?access_token=" + accToken;
	log(requestUrl);
	log(getNewEventPost(eventName, start, end));
	QNetworkRequest req = QNetworkRequest(QUrl(requestUrl));
//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	apiWriterNetMan->post(req,
			getNewEventPost(eventName, start, end, false, timeZone).toUtf8());
	++requestCount;
}

//POST /calendar/v3/calendars/primary/events
//...
//
//{
//  "summary": "Appointment",
//  "location": "Somewhere",
//  "start": {
//    "dateTime": "2011-06-03T10:00:00.000-07:00"
//  },
//  "end": {
//    "dateTime": "2011-06-03T10:25:00.000-07:00"
//  },
//  "attendees": [
//    {
//      "email": "attendeeEmail",
//      # Other attendee's data...
//    },
//    # ...
//  ],
//}
QString GalendarForm::getNewEventPost(const QString &eventName,
		const QString &startDateTime, const QString &endDateTime, bool allday,
		const QString &description, const QString &location,
		const QStringList &attendees, const QString &timeZone) {
	QString result = "{\"summary\":\"" + eventName + "\",\"location\":\""
			+ location + "\",\"description\":\"" + description
			+ "\",\"start\":{\"" + (allday ? "date" : "dateTime") + "\":\""
			+ startDateTime + "\",\"timeZone\":\"" + timeZone
			+ "\"},\"end\":{\"" + (allday ? "date" : "dateTime") + "\":\""
			+ endDateTime + "\",\"timeZone\":\"" + timeZone + "\"}";
	if (attendees.size() > 0) {
		result += ",\"attendees\":[";
		for (int i = 0; i < attendees.size(); ++i) {
			result += "{\"email\":\"" + attendees[i] + "\"}";
			if (i < attendees.size() - 1) {
				result += ",";
			}
		}
		result += "]";
	}
	result += "}";
	return result;
}

QString GalendarForm::getNewCalendarPost(const QString &calendarName,
		const QString &timeZone) {
	QString result = "{\"summary\":\"" + calendarName + "\",\"timeZone\":\""
			+ timeZone + "\"}";
	return result;
}

QString GalendarForm::getCurrentCalendarId() const {
	return ui.calCombo->itemData(ui.calCombo->currentIndex()).toString();
}

QString GalendarForm::getCurrentComboEventId() const {
	return ui.eventCombo->itemData(ui.eventCombo->currentIndex()).toString();
}

bool GalendarForm::prepareDB() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbName);
	if (!db.open()) {
		QMessageBox::critical(0,
				qApp->tr("Cannot open database %1").arg(dbName),
				qApp->tr("Unable to establish a database connection.\n"
						"This example needs SQLite support. Please read "
						"the Qt SQL driver documentation for information how "
						"to build it.\n\n"
						"Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	QSqlQuery query;
	if (!query.exec("CREATE TABLE IF NOT EXISTS Dbinfo ("
			"key TEXT, "
			"value TEXT)")) {
		QMessageBox::critical(0, qApp->tr("Creating Project table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}

	// Check database version
	query.exec("SELECT value FROM Dbinfo "
			"WHERE key=\"version\"");
	if (query.next()) {
		QString version = query.value(0).toString();
		if (version.toInt() > 3) { // Currently supported version
			log("[DB] Unsupported database version " + version);
			QMessageBox::warning(
					this,
					tr("Unsupported database"),
					tr(
							"This database was created by an unsupported version of %1\n"
									"please check <a href=\"http://code.google.com/p/valendar\">project homepage</a> for updates"));
		} else if (version.toInt() == 3) {
			log("[DB] Openning database version 3");
		}
	} else {
		query.prepare("INSERT INTO Dbinfo (key, value) VALUES (:key, :value)");
		query.bindValue(":key", "version");
		query.bindValue(":value", "3");
		query.exec();
	}

	if (!query.exec("CREATE TABLE IF NOT EXISTS Project ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"note TEXT, "
			"creation INTEGER, "
			"attendees TEXT)")) {
		QMessageBox::critical(0, qApp->tr("Creating Project table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	if (!query.exec("CREATE TABLE IF NOT EXISTS Event ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"note TEXT, "
			"start INTEGER, "
			"length INTEGER, "
			"allday INTEGER, "
			"attendees TEXT, "
			"projId INTEGER)")) {
		QMessageBox::critical(0, qApp->tr("Creating Event table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	if (!query.exec("CREATE TABLE IF NOT EXISTS Calman ("
			"id TEXT, "
			"name TEXT, "
			"account TEXT,"
			"creation INTEGER)")) {
		QMessageBox::critical(0, qApp->tr("Creating Calman table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	QSqlTableModel * projectModel = new QSqlTableModel;
//	QSqlTableModel * eventModel = new QSqlTableModel;
	projectModel->setTable("Project");
	if (!projectModel->select()) {
		QMessageBox::critical(0, qApp->tr("Selecting project table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
//	projectModel->removeColumn(0); // DO NOT use this method! It fails model-view auto-sync.
//	projectModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
	projectModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
//	projectModel->setHeaderData(2, Qt::Horizontal, tr("Note"));
//	projectModel->setHeaderData(3, Qt::Horizontal, tr("Created"));
//	projectModel->setHeaderData(4, Qt::Horizontal, tr("Attendees"));

//	eventModel->setTable("event");
	ui.projectView->setModel(projectModel);
	ui.projectView->setModelColumn(1);
//	ui.projectView->hideColumn(0);
//	ui.projectView->setItemDelegate(new ProjectDelegate(this));
	connect(
			ui.projectView->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(selectedProjectChanged()));
//	ui.projectView->selectionModel()->setCurrentIndex()
	selectCalman();
	return true;
}

void GalendarForm::on_projectAddButton_clicked() {
	QSqlQuery query;
	query.prepare("INSERT INTO Project (name, creation, note) "
			"VALUES (:name, :creation, :note)");
	query.bindValue(":name", "Project");
	query.bindValue(":creation", QDateTime::currentDateTime().toTime_t());
	query.bindValue(":note", "Project note");
	query.exec();
	selectProjects();
}

void GalendarForm::on_projectDeleteButton_clicked() {
	QModelIndex curIndex = ui.projectView->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
		QSqlQuery query;
		query.prepare("DELETE FROM Project WHERE id=:projid");
		query.bindValue(":projid", currentProjectId);
		query.exec();
		selectProjects();
	} else {
		log("[INFO] No project deleted: no project selected");
	}
}

void GalendarForm::selectedProjectChanged() {
	QSqlTableModel * projectModel = (QSqlTableModel *) ui.projectView->model();
	int currentRow = ui.projectView->currentIndex().row();
	currentProjectId = projectModel->record(currentRow).value("id").toInt();
	selectEvents();
}

//void GalendarForm::selectedEventChanged() {
//	QSqlTableModel * eventModel = (QSqlTableModel *) ui.eventView->model();
//	int currentRow = ui.eventView->currentIndex().row();
//	currentEventId = eventModel->record(currentRow).value("id").toInt();
//}

void GalendarForm::on_eventAddButton_clicked() {
	QModelIndex curIndex = ui.projectView->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
		QSqlQuery query;
		query.prepare(
				"INSERT INTO Event (name, note, start, length, allday, projid) "
						"VALUES (:name, :note, :start, :length, :allday, :projid)");
		query.bindValue(":name", "Event");
		query.bindValue(":note", "Event note");
		query.bindValue(":start", 9 * 3600);
		query.bindValue(":length", 25 * 60);
		query.bindValue(":allday", 0);
		query.bindValue(":projid", currentProjectId);
		query.exec();
		selectEvents();
	} else {
		log("[INFO] No event added: no project selected");
	}
}

void GalendarForm::selectEvents() {
//	std::cout << "selectEvents()" << std::endl;
	QSqlTableModel * eventModel = new QSqlTableModel;
	eventModel->setTable("Event");
	eventModel->setEditStrategy(QSqlTableModel::OnFieldChange);
	eventModel->setFilter("projid=" + QString::number(currentProjectId));
	if (!eventModel->select()) {
		QMessageBox::critical(0, qApp->tr("Selecting events failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
	}
//	eventModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
	eventModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	eventModel->setHeaderData(2, Qt::Horizontal, tr("Note"));
	eventModel->setHeaderData(3, Qt::Horizontal, tr("Start"));
	eventModel->setHeaderData(4, Qt::Horizontal, tr("Duration"));
	eventModel->setHeaderData(5, Qt::Horizontal, tr("All day?"));
//	eventModel->setHeaderData(6, Qt::Horizontal, tr("Attendees"));

//	eventModel->setTable("event");
	ui.eventView->setModel(eventModel);
	ui.eventView->hideColumn(0);
	ui.eventView->hideColumn(6);
	ui.eventView->hideColumn(7);
	ui.eventView->setItemDelegate(new EventDelegate(this));
//	connect(
//			ui.eventView->selectionModel(),
//			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
//			this, SLOT(selectedEventChanged()));
}

void GalendarForm::selectProjects() {
	QSqlTableModel * projectModel = (QSqlTableModel *) ui.projectView->model();
//	QSqlRecord newRecord;
//	newRecord.setValue("creation",
//			QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//	projectModel->insertRow(projectModel->rowCount());
//	projectModel->record(projectModel->rowCount() - 1).setValue("creation",
//			QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	projectModel->setEditStrategy(QSqlTableModel::OnFieldChange);
	projectModel->select();
//	ui.projectView->setCurrentIndex(projectModel->index(0, 1));
//	ui.projectView->hideColumn(0);
//	ui.projectView->hideColumn(2);
//	ui.projectView->hideColumn(3);
//	ui.projectView->hideColumn(4);
//	ui.projectView->setItemDelegate(new ProjectDelegate(this));
}

void GalendarForm::on_eventDeleteButton_clicked() {
	int curEventId = getCurrentEventId();
	if (curEventId >= 0) {
		QSqlQuery query;
		query.prepare("DELETE FROM Event WHERE id=:eventid");
		query.bindValue(":eventid", getCurrentEventId());
		query.exec();
		selectEvents();
	} else {
		log("[INFO] No event deleted: no event selected");
	}
}

void GalendarForm::closeEvent(QCloseEvent * event) {
	saveSettings();
	QWidget::closeEvent(event);
}

void GalendarForm::on_publishButton_clicked() {
	ui.tabWidget->setCurrentWidget(ui.calendarTab);
}

void GalendarForm::loadSettings() {
	QSettings settings;
	settings.beginGroup("gui");
	restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
//	ui.tabWidget->setCurrentIndex(
//			settings.value("currentTab", ui.tabWidget->currentIndex()).toInt());
	ui.projectSplitter->restoreState(
			settings.value("projectSplitter").toByteArray());
	ui.infoSplitter->restoreState(settings.value("infoSplitter").toByteArray());
	ui.eventSplitter->restoreState(
			settings.value("eventSplitter").toByteArray());
	settings.endGroup();
}

void GalendarForm::saveSettings() {
	QSettings settings;
	settings.beginGroup("gui");
	settings.setValue("geometry", saveGeometry());
//	settings.setValue("currentTab", ui.tabWidget->currentIndex());
	settings.setValue("projectSplitter", ui.projectSplitter->saveState());
	settings.setValue("infoSplitter", ui.infoSplitter->saveState());
	settings.setValue("eventSplitter", ui.eventSplitter->saveState());
	settings.endGroup();
}

void GalendarForm::on_calendarNewButton_clicked() {
	if (accToken.isEmpty()) {
		ui.webView->setContent(tr("Please log in first.").toUtf8(),
				"text/plain");
		return;
	}
	QString calName = QInputDialog::getText(this, tr("Calendar Name"),
			tr("Name the calendar to be published"));
	if (calName.isEmpty()) {
		return;
	}
	createCalendar(calName);
}

void GalendarForm::on_clearDatabaseButton_clicked() {
	QSqlDatabase::database().close();
	QFile::remove(dbName);
	prepareDB();
}

void GalendarForm::createEvent(const QString &calId, const QString &eventName,
		const QString &startDateTime, const QString &endDateTime, bool allday,
		const QString &description, const QString &location,
		const QStringList &attendees, const QString &timeZone) {
	if (accToken.isEmpty()) {
		log("[ERROR] No access token, no event created.");
		return;
	}
	QString requestUrl = calApiBaseUrl + "/calendars/" + calId
			+ "/events?access_token=" + accToken;
	log(requestUrl);
	log(
			getNewEventPost(eventName, startDateTime, endDateTime, allday,
					description, location, attendees, timeZone));
	QNetworkRequest req = QNetworkRequest(QUrl(requestUrl));
//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	apiWriterNetMan->post(
			req,
			getNewEventPost(
					eventName,
					startDateTime,
					endDateTime,
					allday,
					description,
					location,
					attendees,
					(timeZone.isEmpty() ?
							getCalendarTimeZone(getCurrentCalendarId()) :
							timeZone)).toUtf8());
	++requestCount;
}

void GalendarForm::createCalendar(const QString &calendarName,
		const QString &timeZone) {
	if (accToken.isEmpty()) {
		log("[ERROR] No access token, no event created.");
		return;
	}
	QString requestUrl = calApiBaseUrl + "/calendars?access_token=" + accToken;
	log(requestUrl);
	log(getNewCalendarPost(calendarName, timeZone));
	QNetworkRequest req = QNetworkRequest(QUrl(requestUrl));
//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	apiWriterNetMan->post(
			req,
			getNewCalendarPost(
					calendarName,
					(timeZone.isEmpty() ?
							getCalendarTimeZone(getCurrentCalendarId()) :
							timeZone)).toUtf8());
	++requestCount;
}

QScriptValue GalendarForm::getCalendarById(const QString &calId) const {
	QScriptValueIterator it(calendars.property("items"));
	while (it.hasNext()) {
		it.next();
		QString id = it.value().property("id").toString();
		if (id == calId) {
			return it.value();
		}
	}
	return QScriptValue();
}

QString GalendarForm::getCalendarTimeZone(const QString &calId) const {
	return getCalendarById(calId).property("timeZone").toString();
}

void GalendarForm::on_eventListButton_clicked() {
	getAllEvents();
}

void GalendarForm::syncProjectSelection(const QModelIndex & currentIndex) {
//	std::cout << currentIndex.row() << "," << currentIndex.column()
//			<< std::endl;
	ui.projectView->selectionModel()->setCurrentIndex(currentIndex,
			QItemSelectionModel::Select);
	if (currentIndex.isValid()) {
		int row = currentIndex.row();
//		QAbstractItemModel * model = ui.projectView->model();
		ui.projectNoteEdit->setText(
				currentIndex.sibling(row, 2).data().toString());
		ui.projectCreatedEdit->setText(
				QDateTime::fromTime_t(
						currentIndex.sibling(row, 3).data().toInt()).toString());
		ui.projectAttendeeEdit->setText(
				currentIndex.sibling(row, 4).data().toString());
		projectAttributesEdited = false;
	} else {
		ui.projectCreatedEdit->clear();
		ui.projectNoteEdit->clear();
		ui.projectAttendeeEdit->clear();
	}
}

void GalendarForm::contentModified() {
	projectAttributesEdited = true;
}

void GalendarForm::on_projectSaveButton_clicked() {
	if (projectAttributesEdited) {
		QString projNote = ui.projectNoteEdit->toPlainText();
		QString projAttn = getFormattedAttendees(
				ui.projectAttendeeEdit->toPlainText());
		int row = ui.projectView->currentIndex().row();
		QAbstractItemModel * model = ui.projectView->model();
		QModelIndex index = model->index(row, 1);
		if (row >= 0) {
			model->setData(model->index(row, 2), QVariant(projNote));
			model->setData(model->index(row, 4), QVariant(projAttn));
			model->submit();
			ui.projectView->selectionModel()->setCurrentIndex(index,
					QItemSelectionModel::Select);
			syncProjectSelection(index);
		}
		projectAttributesEdited = false;
	}
}

//void GalendarForm::on_formatAttendeeButton_clicked() {
//	QString text = ui.projectAttendeeEdit->toPlainText();
//	ui.projectAttendeeEdit->clear();
//	QTextStream in(&text);
//	QString line = in.readLine();
//	while (!line.isNull()) {
//		if (!line.isEmpty()) {
//			ui.projectAttendeeEdit->append(line.trimmed());
//		}
//		line = in.readLine();
//	}
//}

void GalendarForm::publishCalendarId(const QString &calId) {
	//	QSqlTableModel * eventModel = new QSqlTableModel;
	//	eventModel->setTable("event");

	QSqlQuery query;
	query.prepare("SELECT name, note, start, length, allday FROM Event "
			"WHERE projid=:projid");
	query.bindValue(":projid", currentProjectId);
	query.exec();
	QString summary;
	QString description;
	int start;
	int length;
	QString startString;
	QString endString;
	bool allday;
	while (query.next()) {
		summary = query.value(0).toString();
		description = query.value(1).toString();
		start = query.value(2).toInt();
		length = query.value(3).toInt();
		allday = query.value(4).toBool();
		QDateTime origin = ui.publishDateEdit->dateTime();
		startString = QDateTime::fromTime_t(origin.toTime_t() + start).toString(
				allday ? "yyyy-MM-dd" : "yyyy-MM-ddThh:mm:ss");
		endString =
				QDateTime::fromTime_t(origin.toTime_t() + start + length).toString(
						allday ? "yyyy-MM-dd" : "yyyy-MM-ddThh:mm:ss");
		createEvent(calId, summary, startString, endString, allday,
				description);
	}
	query.prepare("SELECT attendees FROM Project "
			"WHERE id=:projid");
	query.bindValue(":projid", currentProjectId);
	query.exec();
	QString attendees;
	if (query.next()) {
		attendees = query.value(0).toString().trimmed();
		if (!attendees.isEmpty()) {
			createAcl(calId, attendees);
		}
	}
	//	eventModel->setFilter("projid=" + QString::number(currentProjectId));
	//	if (!eventModel->select()) {
	//		QMessageBox::critical(0, qApp->tr("Selecting events failed"),
	//				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
	//	}
	//	eventModel->removeColumn(5); // don't show the ID
	//	eventModel->removeColumn(0); // don't show the ID
	////	eventModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
	//	eventModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
	//	eventModel->setHeaderData(1, Qt::Horizontal, tr("Note"));
	//	eventModel->setHeaderData(2, Qt::Horizontal, tr("Start"));
	//	eventModel->setHeaderData(3, Qt::Horizontal, tr("Duration"));
	//
	////	eventModel->setTable("event");
	//	ui.eventTable->setModel(eventModel);
	////	ui.eventTable->setItemDelegate(new EventDelegate(this));
	//	connect(
	//			ui.eventTable->selectionModel(),
	//			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
	//			this, SLOT(selectedEventChanged()));
}

void GalendarForm::on_publishExistingButton_clicked() {
	if (currentProjectId < 0) {
		log(tr("[INFO] No project selected"));
		ui.webView->setContent(
				tr("No project selected for publishing.").toUtf8(),
				"text/plain");
	}
	if (ui.calCombo->currentIndex() >= 0) {
		publishCalendarId(getCurrentCalendarId());
	} else {
		log(tr("[INFO] No calendar selected"));
		ui.webView->setContent(
				tr("No calendar selected for publishing.").toUtf8(),
				"text/plain");
	}
}

void GalendarForm::on_aclListButton_clicked() {
	if (accToken.isEmpty()) {
		return;
	}
	getAclFromCalendar(getCurrentCalendarId());
}

void GalendarForm::getAclFromCalendar(const QString &calId) {
	QString aclListUrl = calApiBaseUrl + "/calendars/" + calId + "/acl"
			"?access_token=" + accToken;
	log(aclListUrl);
	apiReaderNetMan->get(QNetworkRequest(QUrl(aclListUrl)));
}

QString GalendarForm::getNewAclPost(const QString &attendee,
		const QString &role) {
	QString result = "{\"scope\":{\"type\":\"user\",\"value\":\"" + attendee
			+ "\"},\"role\":\"" + role + "\"}";
	return result;
}

void GalendarForm::createAcl(const QString &calId, const QString &attendees,
		const QString &role) {
	if (accToken.isEmpty()) {
		log("[ERROR] No access token, no event created.");
		ui.webView->setContent(tr("Please log in").toUtf8(), "text/plain");
		return;
	}
	QString requestUrl = calApiBaseUrl + "/calendars/" + calId
			+ "/acl?access_token=" + accToken;
	QStringList list = attendees.split("\n", QString::SkipEmptyParts);
	log(requestUrl);
	foreach(QString attendee, list) {
		log(getNewAclPost(attendee, role));
		QNetworkRequest req = QNetworkRequest(QUrl(requestUrl));
		//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
		req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
		apiWriterNetMan->post(req, getNewAclPost(attendee, role).toUtf8());
		++requestCount;
	}
}

void GalendarForm::on_editEventsButton_clicked() {
	ui.tabWidget->setCurrentWidget(ui.eventTab);
}

bool GalendarForm::eventFilter(QObject *object, QEvent *event) {
	if (event->type() == QEvent::FocusOut
			&& (object == ui.projectNoteEdit || object == ui.projectAttendeeEdit)) {
//		QFocusEvent *focusEvent = static_cast<QFocusEvent *>(event);
//		ui.projectSaveButton->click();
		on_projectSaveButton_clicked();
		return false;
	}
	return false;
}

void GalendarForm::on_backToOverviewButton_clicked() {
	ui.tabWidget->setCurrentWidget(ui.projectTab);
}

void GalendarForm::selectCalman() {
	QSqlTableModel * calmanModel = new QSqlTableModel;
	calmanModel->setTable("Calman");
	if (!calmanModel->select()) {
		QMessageBox::critical(0, qApp->tr("Selecting calman failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
	}
	calmanModel->setHeaderData(1, Qt::Horizontal, tr("Calendar"));
	calmanModel->setHeaderData(2, Qt::Horizontal, tr("Account"));
	calmanModel->setHeaderData(3, Qt::Horizontal, tr("Created"));

	ui.calmanView->setModel(calmanModel);
	ui.calmanView->hideColumn(0);
	ui.calmanView->setItemDelegate(new CalmanDelegate(this));
}

QString GalendarForm::getFormattedAttendees(const QString &attendees) {
	QString formatted = attendees;
	formatted.replace(",", "\n");
	formatted.replace(";", "\n");
	formatted.replace(" ", "\n");
	formatted.replace("\t", "\n");
	while (formatted.contains("\n\n")) {
		formatted.replace("\n\n", "\n");
	}
	return formatted;
}

void GalendarForm::on_calmanDeleteButton_clicked() {
	QModelIndex curIndex = ui.calmanView->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
//		int id = curIndex.sibling(curIndex.row(), 1).data().toInt();
//		log("[DEBUG] Deleting calman with id " + QString::number(id));
//		QSqlQuery query;
//		query.prepare("DELETE FROM Calman WHERE id=:id");
//		query.bindValue(":id", id);
//		query.exec();
		ui.calmanView->model()->removeRow(curIndex.row());
		ui.calmanView->model()->submit();
		selectCalman();
	} else {
		log("[INFO] No calman deleted: no calman selected");
	}
}

void GalendarForm::on_addRowButton_clicked() {
	ui.tableWidget->insertRow(0);
}

void GalendarForm::on_addColButton_clicked() {
	ui.tableWidget->insertColumn(0);
}

int GalendarForm::getCurrentEventId() const {
	QModelIndex curIndex = ui.eventView->currentIndex();
	if (curIndex.isValid()) {
		return curIndex.sibling(curIndex.row(), 0).data().toInt();
	} else {
		return -1;
	}
}

//QString GalendarForm::getUtcOffset() {
//	QDateTime dt = QDateTime::currentDateTime();
//	int offseth = dt.time().hour() - dt.time().hour();
//	int offsetm = dt.time().minute() - dt.time().minute();
//	if (offseth >= 0) {
//		return "+" + QString::number(offseth) + ":" + QString::number(offsetm);
//	} else {
//		return QString::number(offseth) + ":" + QString::number(offsetm);
//	}
//}
