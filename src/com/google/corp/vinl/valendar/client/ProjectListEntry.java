package com.google.corp.vinl.valendar.client;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.rpc.ProjectListService;
import com.google.corp.vinl.valendar.client.rpc.ProjectListServiceAsync;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.RootLayoutPanel;

public class ProjectListEntry implements EntryPoint, EntryThatCanCreate {

	private ProjectListWidget mainWidget;
	private List<Project> projects;
	private int currentIndex;

	static private final ProjectListServiceAsync PLIST_SERVICE = GWT
			.create(ProjectListService.class);

	@Override
	public void onModuleLoad() {
		mainWidget = new ProjectListWidget(this);
		DockLayoutPanel mainPanel = new DockLayoutPanel(Unit.EM);
		RootLayoutPanel.get().add(mainPanel);
		Frame headerFrame = new Frame("header.jsp");
		headerFrame.setStyleName("gwt-FrameHeader");
		mainPanel.addNorth(headerFrame, 4);
		mainPanel.add(mainWidget);
		mainWidget.refreshButton.click();
	}

	public void on_refreshButton_clicked() {
		mainWidget.projectList.clear();
		mainWidget.projectList.addItem("Loading...");
		mainWidget.refreshButton.setEnabled(false);
		PLIST_SERVICE.getProjects(new AsyncCallback<List<Project>>() {
			@Override
			public void onSuccess(List<Project> result) {
				projects = result;
				mainWidget.projectList.clear();
				if (projects != null) {
					int count = 0;
					for (Project p : projects) {
						mainWidget.projectList.addItem(p.getTitle());
						++count;
					}
					if (count == 0) {
						mainWidget.projectList.addItem("(EMPTY)");
					}
				}
				currentIndex = mainWidget.projectList.getSelectedIndex();
				mainWidget.refreshButton.setEnabled(true);
				loadProjectDetail(null);
			}

			@Override
			public void onFailure(Throwable caught) {
				mainWidget.refreshButton.setEnabled(true);
				mainWidget.messageLabel.setText("Refresh failed!");
				Utils.handleFailure(caught);
			}
		});
	}

	// static public void handleFailure(Throwable caught) {
	// if (caught instanceof AuthenticationException) {
	// Window.Location.reload();
	// } else {
	// caught.printStackTrace();
	// Window.alert("ERROR: " + caught.getMessage());
	// }
	// }

	// public void on_createButton_clicked() {
	// String pName = mainWidget.newProjectText.getText();
	// if (!pName.isEmpty()) {
	// Project p = new Project();
	// p.setTitle(pName);
	// p.setCreationDate(new Date());
	// PLIST_SERVICE.addProject(p, new AsyncCallback<Void>() {
	// @Override
	// public void onSuccess(Void result) {
	// mainWidget.newProjectText.setText("");
	// }
	//
	// @Override
	// public void onFailure(Throwable caught) {
	// handleFailure(caught);
	// }
	// });
	// }
	// }

	public void on_projectList_changed() {
		int selectedIndex = mainWidget.projectList.getSelectedIndex();
		if (selectedIndex != currentIndex) {
			currentIndex = selectedIndex;
			loadCurrentProjectDetail();
		}
	}

	private void loadCurrentProjectDetail() {
		if (currentIndex >= 0) {
			loadProjectDetail(projects.get(currentIndex));
		}
	}

	private void loadProjectDetail(Project project) {
		mainWidget.nameText
				.setText(project == null ? "" : (project.getTitle()));
		mainWidget.descriptionText.setText(project == null ? "" : (project
				.getDescription()));
		mainWidget.creationDateLabel.setText(project == null ? ""
				: (DateTimeFormat.getFormat("yyyy-MM-dd HH:mm:ss")
						.format(project.getCreationDate())));
		mainWidget.eventCountLabel.setText(String.valueOf(project == null ? ""
				: project.getEventIds().size()));
		mainWidget.publicationCountLabel.setText(String
				.valueOf(project == null ? "" : project.getPublicationIds()
						.size()));
	}

	@Override
	public void createNew(String projectName) {
		Project project = new Project();
		project.setTitle(projectName);
		final DialogBox waitingDialog = new DialogBox();
		waitingDialog.setHTML("Creating...");
		PLIST_SERVICE.addProject(project, new AsyncCallback<Void>() {
			@Override
			public void onSuccess(Void result) {
				waitingDialog.hide();
				mainWidget.refreshButton.click();
			}

			@Override
			public void onFailure(Throwable caught) {
				waitingDialog.hide();
				Utils.handleFailure(caught);
			}
		});
	}

	public void on_deleteButton_clicked() {
		if (currentIndex >= 0) {
			if (Window.confirm("Delete project? This cannot be undone.")) {
				Project p = projects.get(currentIndex);
				deleteProject(p);
			}
		}
	}

	public void deleteProject(Project project) {
		deleteProject(project.getId());
	}

	public void deleteProject(Long id) {
		mainWidget.messageLabel.setText("Deleting...");
		mainWidget.deleteButton.setEnabled(false);
		PLIST_SERVICE.removeProject(id, new AsyncCallback<Void>() {
			@Override
			public void onSuccess(Void result) {
				mainWidget.deleteButton.setEnabled(true);
				mainWidget.messageLabel.setText("");
				mainWidget.refreshButton.click();
			}

			@Override
			public void onFailure(Throwable caught) {
				mainWidget.deleteButton.setEnabled(true);
				mainWidget.messageLabel.setText("Deleting failed!");
				Utils.handleFailure(caught);
			}
		});
	}

	public void on_saveButton_clicked() {
		if (currentIndex >= 0) {
			Project p = projects.get(currentIndex);
			if (p != null) {
				p.setTitle(mainWidget.nameText.getText());
				p.setDescription(mainWidget.descriptionText.getText());
				saveProject(p);
			}
		}
	}

	private void saveProject(Project project) {
		mainWidget.messageLabel.setText("Saving...");
		mainWidget.saveButton.setEnabled(false);
		PLIST_SERVICE.addProject(project, new AsyncCallback<Void>() {
			@Override
			public void onSuccess(Void result) {
				mainWidget.saveButton.setEnabled(true);
				mainWidget.messageLabel.setText("");
			}

			@Override
			public void onFailure(Throwable caught) {
				mainWidget.saveButton.setEnabled(true);
				mainWidget.messageLabel.setText("Saving failed!");
				Utils.handleFailure(caught);
			}
		});
	}

	public void on_editButton_clicked() {
		if (currentIndex >= 0) {
			Long id = projects.get(currentIndex).getId();
			Window.Location.assign("/pedit.jsp?pid=" + id);
		} else {
			mainWidget.messageLabel.setText("No project selected!");
		}
	}

	public void on_publishButton_clicked() {
		if (currentIndex >= 0) {
			Long id = projects.get(currentIndex).getId();
			Window.Location.assign("/ppublish.jsp?pid=" + id);
		} else {
			mainWidget.messageLabel.setText("No project selected!");
		}
	}

}
