package com.google.corp.vinl.valendar.server;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Attendee;
import com.google.corp.vinl.valendar.client.rpc.ProjectPublishService;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

@SuppressWarnings("serial")
public class ProjectPublishServiceImpl extends RemoteServiceServlet implements
		ProjectPublishService {

	private DAO dao;

	public ProjectPublishServiceImpl() {
		dao = DAO.get();
	}

	@Override
	public List<Attendee> getAttendees(Long pubId) {
		// TODO Auto-generated method stub
		return null;
	}

}
