package com.google.corp.vinl.valendar.client;

import java.util.Collections;
import java.util.List;
import java.util.Stack;

import com.google.corp.vinl.valendar.client.model.Event;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.rpc.ProjectEditService;
import com.google.corp.vinl.valendar.client.rpc.ProjectEditServiceAsync;
import com.google.corp.vinl.valendar.client.rpc.ProjectListService;
import com.google.corp.vinl.valendar.client.rpc.ProjectListServiceAsync;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.RootLayoutPanel;

public class ProjectEditEntry implements EntryPoint, EntryThatCanCreate {

	enum LeaveDirection {
		LD_NOWHERE, LD_PLIST, LD_PPUBLISH
	};

	static final Long MSECS_PER_DAY = 1000L * 60 * 60 * 24;
	static private final ProjectEditServiceAsync PEDIT_SERVICE = GWT
			.create(ProjectEditService.class);
	static private final ProjectListServiceAsync PLIST_SERVICE = GWT
			.create(ProjectListService.class);

	private LeaveDirection leaveDirection;
	private ProjectEditWidget mainWidget;
	private List<Event> events;
	private int currentIndex;
	private Project project;
	// private DialogBox waitingDialog;
	// private boolean leavingPage;
	// private int waitingLevel = 0;
	private Stack<String> waitingMessages = new Stack<String>();

	@Override
	public void onModuleLoad() {
		leaveDirection = LeaveDirection.LD_NOWHERE;
		// leavingPage = false;
		events = null;
		mainWidget = new ProjectEditWidget(this);
		waitingMessages.push("");
		DockLayoutPanel mainPanel = new DockLayoutPanel(Unit.EM);
		RootLayoutPanel.get().add(mainPanel);
		Frame headerFrame = new Frame("header.jsp");
		headerFrame.setStyleName("gwt-FrameHeader");
		mainPanel.addNorth(headerFrame, 4);
		mainPanel.add(mainWidget);
		// Window.addWindowClosingHandler(new ClosingHandler() {
		// @Override
		// public void onWindowClosing(ClosingEvent event) {
		// event.setMessage("Unsaved changes will be lost.");
		// }
		// });
		loadProject();
	}

	public void on_refreshButton_clicked() {
		if (project != null) {
			mainWidget.refreshButton.setEnabled(false);
			mainWidget.eventList.clear();
			mainWidget.eventList.addItem("Loading events...");
			waitingMessages.push("Refreshing...");
			mainWidget.messageLabel.setText(waitingMessages.peek());
			// ++waitingLevel;
			saveAllEvents();
			PEDIT_SERVICE.getEvents(project, new AsyncCallback<List<Event>>() {
				@Override
				public void onSuccess(List<Event> result) {
					// --waitingLevel;
					// if (waitingLevel == 0) {
					waitingMessages.pop();
					mainWidget.messageLabel.setText(waitingMessages.peek());
					// }
					events = result;
					Collections.sort(events);
					mainWidget.eventList.clear();
					if (events != null) {
						int count = 0;
						for (Event e : events) {
							mainWidget.eventList.addItem(e.getTitle());
							++count;
						}
						if (count == 0) {
							mainWidget.eventList.addItem("(EMPTY)");
						}
					}
					currentIndex = mainWidget.eventList.getSelectedIndex();
					mainWidget.refreshButton.setEnabled(true);
					loadEventDetail(null);
				}

				@Override
				public void onFailure(Throwable caught) {
					mainWidget.refreshButton.setEnabled(true);
					// --waitingLevel;
					waitingMessages.pop();
					mainWidget.messageLabel.setText("Refresh failed!");
					Utils.handleFailure(caught);
				}
			});
		}
	}

	@Override
	// Create event
	public void createNew(String eventName) {
		if (project != null) {
			Event event = new Event();
			event.setTitle(eventName);
			waitingMessages.push("Creating...");
			mainWidget.messageLabel.setText(waitingMessages.peek());
			// ++waitingLevel;
			PEDIT_SERVICE.addEvent(project, event, new AsyncCallback<Void>() {
				@Override
				public void onSuccess(Void result) {
					// --waitingLevel;
					waitingMessages.pop();
					// if (waitingLevel == 0) {
					mainWidget.messageLabel.setText(waitingMessages.peek());
					// }
					mainWidget.refreshButton.click();
				}

				@Override
				public void onFailure(Throwable caught) {
					// --waitingLevel;
					waitingMessages.pop();
					mainWidget.messageLabel.setText("Creating failed!");
					Utils.handleFailure(caught);
				}
			});
		}
	}

	public void on_eventList_changed() {
		int selectedIndex = mainWidget.eventList.getSelectedIndex();
		if (selectedIndex != currentIndex) {
			if (currentIndex >= 0) {
				saveCurrentEvent();
			}
			currentIndex = selectedIndex;
			loadCurrentEventDetail();
		}
	}

	private void loadCurrentEventDetail() {
		if (currentIndex >= 0 && events != null && events.size() > 0) {
			loadEventDetail(events.get(currentIndex));
		}
	}

	private void loadEventDetail(Event event) {
		mainWidget.nameText.setText(event == null ? "" : (event.getTitle()));
		mainWidget.descriptionText.setText(event == null ? "" : (event
				.getDescription()));
		if (event == null) {
			mainWidget.offsetIntegerBox.setText("");
			mainWidget.durationIntegerBox.setText("");
		} else {
			Long offset = event.getOffsetMsecs();
			if (offset != null) {
				mainWidget.offsetIntegerBox
						.setValue((int) (offset / MSECS_PER_DAY));
			} else {
				mainWidget.offsetIntegerBox.setText("");
			}
			Long duration = event.getDurationMsecs();
			if (duration != null) {
				mainWidget.durationIntegerBox
						.setValue((int) (duration / MSECS_PER_DAY));
			} else {
				mainWidget.durationIntegerBox.setText("");
			}
		}
	}

	private void loadProject() {
		String pid = Window.Location.getParameter("pid");
		waitingMessages.push("Loading project...");
		mainWidget.messageLabel.setText(waitingMessages.peek());
		// ++waitingLevel;
		PLIST_SERVICE.getProject(Long.valueOf(pid),
				new AsyncCallback<Project>() {
					@Override
					public void onSuccess(Project result) {
						// --waitingLevel;
						waitingMessages.pop();
						// if (waitingLevel == 0) {
						mainWidget.messageLabel.setText(waitingMessages.peek());
						// }
						project = result;
						loadProjectDetail();
					}

					@Override
					public void onFailure(Throwable caught) {
						// --waitingLevel;
						waitingMessages.pop();
						mainWidget.messageLabel.setText("Loading failed!");
						Window.alert("Project loading failed!\n"
								+ "This could be caused by a wrong Project ID or server error, etc.\n"
								+ "Going back to project list.");
						Window.Location.assign("/plist.jsp");
						// Utils.handleFailure(caught);
					}
				});
	}

	private void loadProjectDetail() {
		if (project != null) {
			mainWidget.projectNameText.setText(Utils.strOfObj(project
					.getTitle()));
			mainWidget.projectDescriptionText.setText(Utils.strOfObj(project
					.getDescription()));
			mainWidget.creationDateLabel.setText(Utils.getDateFormat().format(
					project.getCreationDate()));
			mainWidget.refreshButton.click();
		}
	}

	public void on_deleteButton_clicked() {
		if (currentIndex >= 0 && events != null && events.size() > 0) {
			if (Window.confirm("Delete event? This cannot be undone.")) {
				Event e = events.get(currentIndex);
				deleteEvent(e);
			}
		}
	}

	private void deleteEvent(Event e) {
		deleteEvent(e.getId());
	}

	private void deleteEvent(Long id) {
		waitingMessages.push("Deleting...");
		mainWidget.messageLabel.setText(waitingMessages.peek());
		// ++waitingLevel;
		mainWidget.deleteButton.setEnabled(false);
		PEDIT_SERVICE.removeEvent(id, new AsyncCallback<Void>() {
			@Override
			public void onSuccess(Void result) {
				mainWidget.deleteButton.setEnabled(true);
				// --waitingLevel;
				// if (waitingLevel == 0) {
				waitingMessages.pop();
				mainWidget.messageLabel.setText(waitingMessages.peek());
				// }
				mainWidget.refreshButton.click();
			}

			@Override
			public void onFailure(Throwable caught) {
				mainWidget.deleteButton.setEnabled(true);
				// --waitingLevel;
				waitingMessages.pop();
				mainWidget.messageLabel.setText("Deleting failed!");
				Utils.handleFailure(caught);
			}
		});
	}

	public void on_saveButton_clicked() {
		saveAllEvents();
	}

	public void saveCurrentEvent() {
		if (currentIndex >= 0 && events != null && events.size() > 0) {
			Event e = events.get(currentIndex);
			e.setDescription(mainWidget.descriptionText.getText());
			e.setTitle(mainWidget.nameText.getText());
			Integer integer = mainWidget.offsetIntegerBox.getValue();
			if (integer == null) {
				e.setOffsetMsecs(null);
			} else {
				e.setOffsetMsecs(Long.valueOf(integer * MSECS_PER_DAY));
			}
			integer = mainWidget.durationIntegerBox.getValue();
			if (integer == null) {
				e.setDurationMsecs(null);
			} else {
				e.setDurationMsecs(Long.valueOf(integer * MSECS_PER_DAY));
			}
		}
	}

	public void saveAllEvents() {
		if (events != null) {
			saveCurrentEvent();
			waitingMessages.push("Saving events...");
			mainWidget.messageLabel.setText(waitingMessages.peek());
			// ++waitingLevel;
			mainWidget.saveButton.setEnabled(false);
			PEDIT_SERVICE.addEvents(project, events, new AsyncCallback<Void>() {
				@Override
				public void onSuccess(Void result) {
					// --waitingLevel;
					// if (waitingLevel == 0) {
					waitingMessages.pop();
					mainWidget.messageLabel.setText(waitingMessages.peek());
					// }
					mainWidget.saveButton.setEnabled(true);
					switch (leaveDirection) {
					case LD_PLIST:
						Window.Location.assign("/plist.jsp");
						break;
					case LD_PPUBLISH:
						Window.Location.assign("/ppublish.jsp?pid="
								+ project.getId());
						break;
					}
				}

				@Override
				public void onFailure(Throwable caught) {
					// --waitingLevel;
					waitingMessages.pop();
					mainWidget.messageLabel.setText("Saving failed!");
					mainWidget.saveButton.setEnabled(true);
					Utils.handleFailure(caught);
				}
			});
		}
	}

	public void on_backButton_clicked() {
		leaveDirection = LeaveDirection.LD_PLIST;
		mainWidget.saveButton.click();
	}

	public void on_publishButton_clicked() {
		leaveDirection = LeaveDirection.LD_PPUBLISH;
		mainWidget.saveButton.click();
	}

}
