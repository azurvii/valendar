package com.google.corp.vinl.valendar.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.Widget;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.uibinder.client.UiHandler;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ClickEvent;

public class ProjectPublishWidget extends Composite {

	private static ProjectPublishWidgetUiBinder uiBinder = GWT
			.create(ProjectPublishWidgetUiBinder.class);

	private ProjectPublishEntry entry;
	@UiField
	Button backButton;
	@UiField
	ListBox publicationList;
	@UiField
	Button editButton;
	@UiField
	Label publicationTimeLabel;
	@UiField
	TextArea attendeesText;
	@UiField
	Button publishButton;
	@UiField
	Label calendarNameLabel;
	@UiField
	ListBox attendeesLabel;
	@UiField
	Label projectEventCountLabel;
	@UiField
	Label projectCreationDateLabel;
	@UiField
	Label messageLabel;
	@UiField
	Label projectNameLabel;
	@UiField
	Label projectDescriptionLabel;
	@UiField
	Button refreshButton;

	interface ProjectPublishWidgetUiBinder extends
			UiBinder<Widget, ProjectPublishWidget> {
	}

	public ProjectPublishWidget(ProjectPublishEntry entry) {
		initWidget(uiBinder.createAndBindUi(this));
		this.entry = entry;
	}

	@UiHandler("publicationList")
	void onPublicationListChange(ChangeEvent event) {
		entry.on_publicationList_changed();
	}

	@UiHandler("addButton")
	void onAddButtonClick(ClickEvent event) {
		entry.on_addButton_clicked();
	}

	@UiHandler("refreshButton")
	void onRefreshButtonClick(ClickEvent event) {
		entry.on_refreshButton_clicked();
	}

	@UiHandler("editButton")
	void onEditButtonClick(ClickEvent event) {
		entry.on_editButton_clicked();
	}

	@UiHandler("backButton")
	void onBackButtonClick(ClickEvent event) {
		entry.on_backButton_clicked();
	}

	@UiHandler("publishButton")
	void onPublishButtonClick(ClickEvent event) {
		entry.on_publishButton_clicked();
	}
}
