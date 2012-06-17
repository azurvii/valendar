package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Event;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.gwt.user.client.rpc.AsyncCallback;

public interface ProjectEditServiceAsync {

	void removeEvent(Event event, AsyncCallback<Void> callback);

	void removeEvent(Long id, AsyncCallback<Void> callback);

	void getEvent(Long id, AsyncCallback<Event> callback);

	void getEvents(Project project, AsyncCallback<List<Event>> callback);

	void addEvent(Project project, Event event, AsyncCallback<Void> callback);

	void addEvents(Project project, List<Event> events,
			AsyncCallback<Void> callback);

}
