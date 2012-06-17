package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Attendee;
import com.google.gwt.user.client.rpc.AsyncCallback;

public interface ProjectPublishServiceAsync {

	void getAttendees(Long pubId, AsyncCallback<List<Attendee>> callback);

}
