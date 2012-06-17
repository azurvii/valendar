package com.google.corp.vinl.valendar.client;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.RootLayoutPanel;

public class ProjectPublishEntry implements EntryPoint {

	ProjectPublishWidget mainWidget;

	@Override
	public void onModuleLoad() {
		mainWidget = new ProjectPublishWidget(this);
		DockLayoutPanel mainPanel = new DockLayoutPanel(Unit.EM);
		RootLayoutPanel.get().add(mainPanel);
		Frame headerFrame = new Frame("header.jsp");
		headerFrame.setStyleName("gwt-FrameHeader");
		mainPanel.addNorth(headerFrame, 4);
		mainPanel.add(mainWidget);
		// ChangeHandler changeSaver = new ChangeHandler() {
		// @Override
		// public void onChange(ChangeEvent event) {
		// saveCurrentProject();
		// }
		// };
		// mainWidget.nameText.addChangeHandler(changeSaver);
		// mainWidget.descriptionText.addChangeHandler(changeSaver);
	}

}
