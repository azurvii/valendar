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
#include "ProjectDelegate.h"

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
			"https://www.googleapis.com/auth/calendar";
	calApiBaseUrl = "https://www.googleapis.com/calendar/v3";
	currentProjectId = -1;
	currentEventId = -1;
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
	if (!debugMode) {
		ui.debugModeCheck->setVisible(false);
	}
//	ui.afterAuthWidget->setEnabled(false);
//	ui.afterCalListWidget->setEnabled(false);
//	ui.eventTable->setItemDelegate(new EventDelegate(this));
//	ui.projectTable->setItemDelegate(new ProjectDelegate(this));
	ui.publishDateTimeEdit->setDate(QDate::currentDate());
	setDebugWidgetsVisible(debugMode);
	prepareDB();
}

void GalendarForm::makeConnections() {
	connect(ui.webView->page()->networkAccessManager(),
			SIGNAL(finished(QNetworkReply *)), this,
			SLOT(readReply(QNetworkReply *)));
	connect(ui.webView, SIGNAL(loadProgress(int)), ui.progressBar,
			SLOT(setValue(int)));
	connect(ui.testWebView, SIGNAL(loadProgress(int)), ui.testProgressBar,
			SLOT(setValue(int)));
	connect(accTokenNetMan, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(readAccessToken(QNetworkReply *)));
	connect(apiReaderNetMan, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(readApiReaderReply(QNetworkReply *)));
	connect(apiWriterNetMan, SIGNAL(finished(QNetworkReply *)), this,
			SLOT(readApiWriterReply(QNetworkReply *)));
}

void GalendarForm::readReply(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
//	log(bytes);
	log(tr("[REQUEST] %1").arg(reply->request().url().toString()));
//	foreach(QByteArray header, reply->request().rawHeaderList()) {
//		log("[HEADER] " + header + "==>" + reply->request().rawHeader(header));
//	}
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
			ui.webView->load(QNetworkRequest(QUrl("about:blank")));
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
	if (visible) {
		ui.tabWidget->addTab(ui.logTab, tr("Log"));
		ui.tabWidget->addTab(ui.testTab, tr("Test"));
		ui.tabWidget->addTab(ui.scriptTab, tr("Script"));
		ui.tabWidget->addTab(ui.pubTab, tr("Publication"));
	} else {
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.logTab));
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.testTab));
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.scriptTab));
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.pubTab));
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
		log(tr("Calendars: (+read-write; -read-only)"));
//			calendars.clear();
		while (it.hasNext()) {
			it.next();
			QString accessRole = it.value().property("accessRole").toString();
			QString calId = it.value().property("id").toString();
			QString calName = it.value().property("summary").toString();
			if (accessRole == "writer" || accessRole == "owner") {
				ui.calCombo->addItem(calName, calId);
				log("+ " + calId + " --> " + calName);
			} else {
				log("- " + calId + " --> " + calName);
			}
		}
//		ui.afterCalListWidget->setEnabled(true);
		if (!debugMode) {
			ui.webView->setContent(
					tr("Please select a calendar from the combo box "
							"in which you would like to publish the project.").toUtf8(),
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
}

void GalendarForm::readApiWriterReply(QNetworkReply *reply) {
	QByteArray bytes = reply->readAll();
	if (debugMode) {
		ui.webView->setContent(bytes, "text/plain");
	}
	QScriptValue result = scriptEngine.evaluate("(" + QString(bytes) + ")");
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
			ui.webView->setContent(tr("Project published.").toUtf8(),
					"text/plain");
		}
	}
}

void GalendarForm::on_calCombo_currentIndexChanged(const QString & calId) {
	log(tr("[DEBUG] on_calCombo_currentIndexChanged(%1)").arg(calId));
//	getAllEvents();
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
			getNewEventPost(eventName, start, end, timeZone).toUtf8());
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
		const QString &startDateTime, const QString &endDateTime,
		const QString &description, const QString &location,
		const QStringList &attendees, const QString &timeZone) {
	QString result = "{\"summary\":\"" + eventName + "\",\"location\":\""
			+ location + "\",\"description\":\"" + description
			+ "\",\"start\":{\"dateTime\":\"" + startDateTime
			+ "\",\"timeZone\":\"" + timeZone + "\"},\"end\":{\"dateTime\":\""
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

QString GalendarForm::getCurrentCalendarId() const {
	return ui.calCombo->itemData(ui.calCombo->currentIndex()).toString();
}

QString GalendarForm::getCurrentEventId() const {
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
	if (!query.exec("CREATE TABLE IF NOT EXISTS Project ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"creation TEXT, "
			"note TEXT)")) {
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
			"attendees TEXT, "
			"projId INTEGER)")) {
		QMessageBox::critical(0, qApp->tr("Creating Event table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	if (!query.exec("CREATE TABLE IF NOT EXISTS Publication ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"pubId TEXT, "
			"gEventId TEXT)")) {
		QMessageBox::critical(0, qApp->tr("Creating Publication table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	if (!query.exec("CREATE TABLE IF NOT EXISTS EventSync ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"pubId INTEGER, "
			"account TEXT, "
			"gCalId TEXT, "
			"gEventId TEXT)")) {
		QMessageBox::critical(0, qApp->tr("Creating EventSync table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	QSqlTableModel * projectModel = new QSqlTableModel;
//	QSqlTableModel * eventModel = new QSqlTableModel;
	projectModel->setTable("project");
	if (!projectModel->select()) {
		QMessageBox::critical(0, qApp->tr("Selecting project table failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
//	projectModel->removeColumn(0); // don't show the ID
	projectModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
	projectModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	projectModel->setHeaderData(2, Qt::Horizontal, tr("Created"));
	projectModel->setHeaderData(3, Qt::Horizontal, tr("Note"));

//	eventModel->setTable("event");
	ui.projectTable->setModel(projectModel);
	ui.projectTable->hideColumn(0);
//	ui.projectTable->setItemDelegate(new ProjectDelegate(this));
	connect(
			ui.projectTable->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(selectedProjectChanged()));
	return true;
}

void GalendarForm::on_projectAddButton_clicked() {
	QSqlQuery query;
	query.prepare("INSERT INTO project (name, creation, note) "
			"VALUES (:name, :creation, :note)");
	query.bindValue(":name", "Project");
	query.bindValue(":creation",
			QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	query.bindValue(":note", "Project note");
	query.exec();
	selectProjects();
}

void GalendarForm::on_projectDeleteButton_clicked() {
	QModelIndex curIndex = ui.projectTable->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
		QSqlQuery query;
		query.prepare("DELETE FROM project WHERE id=:projid");
		query.bindValue(":projid", currentProjectId);
		query.exec();
		selectProjects();
	} else {
		log("[INFO] No project deleted: no project selected");
	}
}

void GalendarForm::selectedProjectChanged() {
	QSqlTableModel * projectModel = (QSqlTableModel *) ui.projectTable->model();
	int currentRow = ui.projectTable->currentIndex().row();
	currentProjectId = projectModel->record(currentRow).value("id").toInt();
	selectEvents();
}

void GalendarForm::selectedEventChanged() {
	QSqlTableModel * eventModel = (QSqlTableModel *) ui.eventTable->model();
	int currentRow = ui.eventTable->currentIndex().row();
	currentEventId = eventModel->record(currentRow).value("id").toInt();
}

void GalendarForm::on_eventAddButton_clicked() {
	QModelIndex curIndex = ui.projectTable->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
		QSqlQuery query;
		query.prepare("INSERT INTO event (name, note, start, length, projid) "
				"VALUES (:name, :note, :start, :length, :projid)");
		query.bindValue(":name", "Event");
		query.bindValue(":note", "Event note");
		query.bindValue(":start", 0);
		query.bindValue(":length", 25 * 60);
		query.bindValue(":projid", currentProjectId);
		query.exec();
		selectEvents();
	} else {
		log("[INFO] No event added: no project selected");
	}
}

void GalendarForm::selectEvents() {
	QSqlTableModel * eventModel = new QSqlTableModel;
	eventModel->setTable("event");
	eventModel->setFilter("projid=" + QString::number(currentProjectId));
	if (!eventModel->select()) {
		QMessageBox::critical(0, qApp->tr("Selecting events failed"),
				qApp->tr("Click Cancel to exit."), QMessageBox::Cancel);
	}
//	eventModel->removeColumn(5); // don't show the ID
//	eventModel->removeColumn(0); // don't show the ID
//	eventModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
	eventModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	eventModel->setHeaderData(2, Qt::Horizontal, tr("Note"));
	eventModel->setHeaderData(3, Qt::Horizontal, tr("Start"));
	eventModel->setHeaderData(4, Qt::Horizontal, tr("Duration"));

//	eventModel->setTable("event");
	ui.eventTable->setModel(eventModel);
	ui.eventTable->hideColumn(0);
	ui.eventTable->hideColumn(5);
	ui.eventTable->setItemDelegate(new EventDelegate(this));
	connect(
			ui.eventTable->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(selectedEventChanged()));
}

void GalendarForm::selectProjects() {
	QSqlTableModel * projectModel = (QSqlTableModel *) ui.projectTable->model();
//	QSqlRecord newRecord;
//	newRecord.setValue("creation",
//			QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//	projectModel->insertRow(projectModel->rowCount());
//	projectModel->record(projectModel->rowCount() - 1).setValue("creation",
//			QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	projectModel->select();
	ui.projectTable->hideColumn(0);
	ui.projectTable->setItemDelegate(new ProjectDelegate(this));
}

void GalendarForm::on_eventDeleteButton_clicked() {
	QModelIndex curIndex = ui.eventTable->selectionModel()->currentIndex();
	if (curIndex.isValid()) {
		QSqlQuery query;
		query.prepare("DELETE FROM event WHERE id=:eventid");
		query.bindValue(":eventid", currentEventId);
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
	ui.tabWidget->setCurrentIndex(
			settings.value("currentTab", ui.tabWidget->currentIndex()).toInt());
	settings.endGroup();
}

void GalendarForm::saveSettings() {
	QSettings settings;
	settings.beginGroup("gui");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("currentTab", ui.tabWidget->currentIndex());
	settings.endGroup();
}

void GalendarForm::on_publishProjectButton_clicked() {
//	QSqlTableModel * eventModel = new QSqlTableModel;
//	eventModel->setTable("event");
	QSqlQuery query;
	query.prepare("SELECT name, note, start, length FROM Event "
			"WHERE projid=:projid");
	query.bindValue(":projid", currentProjectId);
	query.exec();
	QString summary;
	QString description;
	int start;
	int length;
	QString startString;
	QString endString;
	while (query.next()) {
		summary = query.value(0).toString();
		description = query.value(1).toString();
		start = query.value(2).toInt();
		length = query.value(3).toInt();
		QDateTime origin = ui.publishDateTimeEdit->dateTime();
		startString = QDateTime::fromTime_t(origin.toTime_t() + start).toString(
				"yyyy-MM-ddThh:mm:ss");
		endString =
				QDateTime::fromTime_t(origin.toTime_t() + start + length).toString(
						"yyyy-MM-ddThh:mm:ss");
		createEvent(summary, startString, endString, description);
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

void GalendarForm::on_clearDatabaseButton_clicked() {
	QSqlDatabase::database().close();
	QFile::remove(dbName);
	prepareDB();
}

void GalendarForm::createEvent(const QString &eventName,
		const QString &startDateTime, const QString &endDateTime,
		const QString &description, const QString &location,
		const QStringList &attendees, const QString &timeZone) {
	if (accToken.isEmpty()) {
		log("[ERROR] No access token, no event created.");
		return;
	}
	QString requestUrl = calApiBaseUrl + "/calendars/" + getCurrentCalendarId()
			+ "/events?access_token=" + accToken;
	log(requestUrl);
	log(
			getNewEventPost(eventName, startDateTime, endDateTime, description,
					location, attendees, timeZone));
	QNetworkRequest req = QNetworkRequest(QUrl(requestUrl));
//	req.setRawHeader("Authorization", ("Bearer " + accToken).toUtf8());
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	apiWriterNetMan->post(
			req,
			getNewEventPost(
					eventName,
					startDateTime,
					endDateTime,
					description,
					location,
					attendees,
					(timeZone.isEmpty() ?
							getCalendarTimeZone(getCurrentCalendarId()) :
							timeZone)).toUtf8());
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
