package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Attendee;
import com.google.gwt.user.client.rpc.RemoteService;

public interface ProjectPublishService extends RemoteService {

	public List<Attendee> getAttendees(Long pubId);

}
