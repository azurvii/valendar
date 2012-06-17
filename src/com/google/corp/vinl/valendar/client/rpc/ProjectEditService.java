package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Event;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("../peditService")
public interface ProjectEditService extends RemoteService {

	public void addEvent(Project project, Event event);

	public void addEvents(Project project, List<Event> events);

	public void removeEvent(Event event);

	public void removeEvent(Long id);

	public List<Event> getEvents(Project project);

	public Event getEvent(Long id);

}
