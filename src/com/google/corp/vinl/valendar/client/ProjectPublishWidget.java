package com.google.corp.vinl.valendar.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.Widget;

public class ProjectPublishWidget extends Composite {

	private static ProjectPublishWidgetUiBinder uiBinder = GWT
			.create(ProjectPublishWidgetUiBinder.class);

	private ProjectPublishEntry entry;

	// @UiField
	// Button editButton;

	interface ProjectPublishWidgetUiBinder extends
			UiBinder<Widget, ProjectPublishWidget> {
	}

	public ProjectPublishWidget(ProjectPublishEntry entry) {
		initWidget(uiBinder.createAndBindUi(this));
		this.entry = entry;
	}

}
