package com.google.corp.vinl.valendar.client;

import java.util.List;
import java.util.Map;
import java.util.Stack;

import com.google.corp.vinl.valendar.client.model.Attendee;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.model.Publication;
import com.google.corp.vinl.valendar.client.rpc.ProjectListService;
import com.google.corp.vinl.valendar.client.rpc.ProjectListServiceAsync;
import com.google.corp.vinl.valendar.client.rpc.ProjectPublishService;
import com.google.corp.vinl.valendar.client.rpc.ProjectPublishServiceAsync;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.RootLayoutPanel;

public class ProjectPublishEntry implements EntryPoint {

	static private final ProjectPublishServiceAsync PPUBLISH_SERVICE = GWT
			.create(ProjectPublishService.class);
	static private final ProjectListServiceAsync PLIST_SERVICE = GWT
			.create(ProjectListService.class);

	private ProjectPublishWidget mainWidget;
	private Stack<String> waitingMessages = new Stack<String>();
	private List<Publication> publications;
//	private Map<Publication, List<Attendee>> pubAttendees;
	private Project project;

	@Override
	public void onModuleLoad() {
		waitingMessages.push("");
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
		loadProject();
	}

	public void on_backButton_clicked() {
		Window.Location.assign("/plist.jsp");
	}

	public void on_editButton_clicked() {
		Window.Location.assign("/pedit.jsp?pid=" + project.getId());
	}

	public void on_publicationList_changed() {
		// TODO Auto-generated method stub
	}

	public void on_publishButton_clicked() {
		// TODO Auto-generated method stub
	}

	public void on_refreshButton_clicked() {
		// TODO Auto-generated method stub

	}

	public void on_addButton_clicked() {
		// TODO Auto-generated method stub

	}

	private void loadProject() {
		String pid = Window.Location.getParameter("pid");
		waitingMessages.push("Loading project...");
		mainWidget.messageLabel.setText(waitingMessages.peek());
		PLIST_SERVICE.getProject(Long.valueOf(pid),
				new AsyncCallback<Project>() {
					@Override
					public void onSuccess(Project result) {
						waitingMessages.pop();
						mainWidget.messageLabel.setText(waitingMessages.peek());
						project = result;
						loadProjectDetail();
					}

					@Override
					public void onFailure(Throwable caught) {
						waitingMessages.pop();
						mainWidget.messageLabel.setText("Loading failed!");
						Window.alert("Project loading failed!\n"
								+ "This could be caused by a wrong Project ID or server error, etc.\n"
								+ "Going back to project list.");
						Window.Location.assign("/plist.jsp");
					}
				});
	}

	private void loadProjectDetail() {
		if (project != null) {
			mainWidget.projectNameLabel.setText(Utils.strOfObj(project
					.getTitle()));
			mainWidget.projectDescriptionLabel.setText(Utils.strOfObj(project
					.getDescription()));
			mainWidget.projectCreationDateLabel.setText(Utils.getDateFormat()
					.format(project.getCreationDate()));
			mainWidget.projectEventCountLabel.setText(String.valueOf(project
					.getEventIds().size()));
			mainWidget.refreshButton.click();
		}
	}

	private void loadPublicationDetail(Publication publication) {
		if (publication != null) {
			mainWidget.publicationTimeLabel.setText(publication == null ? ""
					: (Utils.getDateFormat().format(publication
							.getPublicationDate())));
			mainWidget.calendarNameLabel.setText(publication == null ? ""
					: publication.getCalendarId());
			if (publication.getAttendeeIds().size() >= 0) {
				PPUBLISH_SERVICE.getAttendees(publication.getId(),
						new AsyncCallback<List<Attendee>>() {
							@Override
							public void onSuccess(List<Attendee> result) {
								// TODO Auto-generated method stub
							}

							@Override
							public void onFailure(Throwable caught) {
								// TODO Auto-generated method stub
							}
						});
			}
		}
	}
}
