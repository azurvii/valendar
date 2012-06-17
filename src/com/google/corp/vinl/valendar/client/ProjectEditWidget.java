package com.google.corp.vinl.valendar.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.uibinder.client.UiHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.Widget;
import com.google.gwt.user.client.ui.IntegerBox;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.Label;

public class ProjectEditWidget extends Composite {

	private static ProjectEditWidgetUiBinder uiBinder = GWT
			.create(ProjectEditWidgetUiBinder.class);

	private NewDialog newDialog;
	private ProjectEditEntry entry;

	@UiField
	Button backButton;
	@UiField
	ListBox eventList;
	@UiField
	Button saveButton;
	@UiField
	Button addButton;
	@UiField
	Button deleteButton;
	@UiField
	Button refreshButton;
	@UiField
	Label messageLabel;
	@UiField
	TextBox nameText;
	@UiField
	TextArea descriptionText;
	@UiField
	IntegerBox offsetIntegerBox;
	@UiField
	IntegerBox durationIntegerBox;
	@UiField
	Label projectNameText;
	@UiField
	Label projectDescriptionText;
	@UiField
	Label creationDateLabel;

	interface ProjectEditWidgetUiBinder extends
			UiBinder<Widget, ProjectEditWidget> {
	}

	public ProjectEditWidget(ProjectEditEntry entry) {
		initWidget(uiBinder.createAndBindUi(this));
		this.entry = entry;
		newDialog = new NewDialog(entry);
	}

	@UiHandler("addButton")
	void onAddButtonClick(ClickEvent event) {
		newDialog.setGlassEnabled(true);
		newDialog.setAnimationEnabled(true);
		newDialog.center();
		newDialog.setHTML("New event");
		newDialog.show();
		newDialog.nameText.setFocus(true);
	}

	@UiHandler("deleteButton")
	void onDeleteButtonClick(ClickEvent event) {
		entry.on_deleteButton_clicked();
	}

	@UiHandler("refreshButton")
	void onRefreshButtonClick(ClickEvent event) {
		entry.on_refreshButton_clicked();
	}

	@UiHandler("saveButton")
	void onSaveButtonClick(ClickEvent event) {
		entry.on_saveButton_clicked();
	}

	@UiHandler("publishButton")
	void onPublishButtonClick(ClickEvent event) {
		entry.on_publishButton_clicked();
	}

	@UiHandler("eventList")
	void onEventListChange(ChangeEvent event) {
		entry.on_eventList_changed();
	}

	@UiHandler("backButton")
	void onBackButtonClick(ClickEvent event) {
		entry.on_backButton_clicked();
	}
}
