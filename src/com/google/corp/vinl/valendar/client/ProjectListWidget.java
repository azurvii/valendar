package com.google.corp.vinl.valendar.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.uibinder.client.UiHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;

public class ProjectListWidget extends Composite {

	final ProjectListEntry entry;
	final NewDialog newDialog;

	private static ProjectListWidgetUiBinder uiBinder = GWT
			.create(ProjectListWidgetUiBinder.class);

	@UiField
	Button publishButton;
	@UiField
	TextArea descriptionText;
	@UiField
	TextBox nameText;
	@UiField
	ListBox projectList;
	@UiField
	Button refreshButton;
	@UiField
	Button editButton;
	@UiField
	Label creationDateLabel;
	@UiField
	Button deleteButton;
	@UiField
	Button saveButton;
	@UiField
	Label messageLabel;
	@UiField
	Label eventCountLabel;
	@UiField
	Label publicationCountLabel;

	// @UiField
	// Label creationDateLabel;

	interface ProjectListWidgetUiBinder extends
			UiBinder<Widget, ProjectListWidget> {
	}

	public ProjectListWidget(ProjectListEntry entry) {
		initWidget(uiBinder.createAndBindUi(this));
		this.entry = entry;
		newDialog = new NewDialog(entry);
	}

	@UiHandler("refreshButton")
	void onRefreshButtonClick(ClickEvent event) {
		entry.on_refreshButton_clicked();
	}

	@UiHandler("editButton")
	void onEditButtonClick(ClickEvent event) {
		entry.on_editButton_clicked();
	}

	@UiHandler("publishButton")
	void onPublishButtonClick(ClickEvent event) {
		entry.on_publishButton_clicked();
	}

	@UiHandler("projectList")
	void onProjectListChange(ChangeEvent event) {
		entry.on_projectList_changed();
	}

	@UiHandler("addButton")
	void onAddButtonClick(ClickEvent event) {
		newDialog.setGlassEnabled(true);
		newDialog.setAnimationEnabled(true);
		newDialog.center();
		newDialog.setHTML("New project");
		newDialog.show();
		newDialog.nameText.setFocus(true);
	}

	@UiHandler("deleteButton")
	void onDeleteButtonClick(ClickEvent event) {
		entry.on_deleteButton_clicked();
	}

	@UiHandler("saveButton")
	void onSaveButtonClick(ClickEvent event) {
		entry.on_saveButton_clicked();
	}
}
