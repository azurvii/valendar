package com.google.corp.vinl.valendar.client.model;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.Entity;
import javax.persistence.Id;

@SuppressWarnings("serial")
@Entity
public class Attendee implements Serializable {

	@Id
	private Long id;
	private String email;
	private String name;
	private Set<Long> publicationIds;

	public Attendee() {
		publicationIds = new HashSet<Long>();
	}

	public Long getId() {
		return id;
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public Set<Long> getPublicationIds() {
		return publicationIds;
	}

	public void setPublicationIds(Set<Long> publicationIds) {
		this.publicationIds = publicationIds;
	}

	public void addPublication(Long publicationId) {
		publicationIds.add(publicationId);
	}

	public void removePublication(Long publicationId) {
		publicationIds.remove(publicationId);
	}

}
