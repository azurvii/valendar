package com.google.corp.vinl.valendar.client.model;

import java.io.Serializable;
import java.util.Date;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.Entity;
import javax.persistence.Id;

@SuppressWarnings("serial")
@Entity
public class Project implements Serializable {

	@Id
	private Long id;
	private String title;
	private String description;
	private Date creationDate;
	private Set<Long> eventIds;
	private Set<Long> publicationIds;

	public Project() {
		eventIds = new HashSet<Long>();
		publicationIds = new HashSet<Long>();
	}

	public Long getId() {
		return id;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public Date getCreationDate() {
		return creationDate;
	}

	public void setCreationDate(Date creationDate) {
		this.creationDate = creationDate;
	}

	public Set<Long> getEventIds() {
		return eventIds;
	}

	public void setEventIds(Set<Long> eventIds) {
		this.eventIds = eventIds;
	}

	public Set<Long> getPublicationIds() {
		return publicationIds;
	}

	public void setPublicationIds(Set<Long> publicationIds) {
		this.publicationIds = publicationIds;
	}

	public void addEvent(Long eventId) {
		eventIds.add(eventId);
	}

	public void addPublication(Long publicationId) {
		publicationIds.add(publicationId);
	}

	public void removeEvent(Long eventId) {
		eventIds.remove(eventId);
	}

	public void removePublication(Long publicationId) {
		publicationIds.remove(publicationId);
	}

}
