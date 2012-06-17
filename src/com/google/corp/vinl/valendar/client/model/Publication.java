package com.google.corp.vinl.valendar.client.model;

import java.io.Serializable;
import java.util.Date;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.Entity;
import javax.persistence.Id;

@SuppressWarnings("serial")
@Entity
public class Publication implements Serializable {

	@Id
	private Long id;
	private Date publicationDate;
	private Long projectId;
	private String calendarId;
	private Set<Long> attendeeIds;

	public Publication() {
		attendeeIds = new HashSet<Long>();
	}

	public Long getId() {
		return id;
	}

	public Date getPublicationDate() {
		return publicationDate;
	}

	public void setPublicationDate(Date publicationDate) {
		this.publicationDate = publicationDate;
	}

	public Long getProjectId() {
		return projectId;
	}

	public void setProjectId(Long projectId) {
		this.projectId = projectId;
	}

	public String getCalendarId() {
		return calendarId;
	}

	public void setCalendarId(String calendarId) {
		this.calendarId = calendarId;
	}

	public Set<Long> getAttendeeIds() {
		return attendeeIds;
	}

	public void setAttendeeIds(Set<Long> attendeeIds) {
		this.attendeeIds = attendeeIds;
	}

	public void addAttendee(Long attendeeId) {
		attendeeIds.add(attendeeId);
	}

	public void removeAttendee(Long attendeeId) {
		attendeeIds.remove(attendeeId);
	}

}
