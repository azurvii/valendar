package com.google.corp.vinl.valendar.server;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Event;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.rpc.ProjectEditService;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

@SuppressWarnings("serial")
public class ProjectEditServiceImpl extends RemoteServiceServlet implements
		ProjectEditService {

	private DAO dao;

	public ProjectEditServiceImpl() {
		dao = DAO.get();
	}

	@Override
	public void addEvent(Project project, Event event) {
		if (project != null) {
			dao.put(project.getId(), event);
		}
	}

	@Override
	public void removeEvent(Event event) {
		dao.remove(event);
	}

	@Override
	public void removeEvent(Long id) {
		dao.removeEvent(id);
	}

	@Override
	public Event getEvent(Long id) {
		return dao.getEvent(id);
	}

	@Override
	public List<Event> getEvents(Project project) {
		return dao.getEvents(project);
	}

	@Override
	public void addEvents(Project project, List<Event> events) {
		if (project != null) {
			dao.put(project.getId(), events);
		}
	}

}
