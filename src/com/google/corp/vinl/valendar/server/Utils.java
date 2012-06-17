package com.google.corp.vinl.valendar.server;

import java.io.IOException;
import java.io.InputStream;
import java.util.Collections;

import javax.jdo.JDOHelper;
import javax.jdo.PersistenceManager;
import javax.jdo.PersistenceManagerFactory;
import javax.servlet.http.HttpServletRequest;

import com.google.api.client.auth.oauth2.Credential;
import com.google.api.client.extensions.appengine.auth.oauth2.AppEngineCredentialStore;
import com.google.api.client.extensions.appengine.http.urlfetch.UrlFetchTransport;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeFlow;
import com.google.api.client.googleapis.auth.oauth2.GoogleClientSecrets;
import com.google.api.client.http.GenericUrl;
import com.google.api.client.http.HttpTransport;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.jackson.JacksonFactory;
import com.google.api.services.calendar.Calendar;
import com.google.api.services.calendar.CalendarScopes;
import com.google.appengine.api.users.UserServiceFactory;
import com.google.common.base.Preconditions;

public class Utils {

	private static final PersistenceManagerFactory pmfInstance = JDOHelper
			.getPersistenceManagerFactory("transactions-optional");

	/** Global instance of the HTTP transport. */
	static final HttpTransport HTTP_TRANSPORT = new UrlFetchTransport();

	/** Global instance of the JSON factory. */
	static final JsonFactory JSON_FACTORY = new JacksonFactory();

	private static final String RESOURCE_LOCATION = "/client_secrets.json";

	private static GoogleClientSecrets clientSecrets = null;

	static GoogleClientSecrets getClientCredential() throws IOException {
		if (clientSecrets == null) {
			InputStream inputStream = Utils.class
					.getResourceAsStream(RESOURCE_LOCATION);
			Preconditions.checkNotNull(inputStream, "missing resource %s",
					RESOURCE_LOCATION);
			clientSecrets = GoogleClientSecrets.load(JSON_FACTORY, inputStream);
			Preconditions
					.checkArgument(
							!clientSecrets.getDetails().getClientId()
									.startsWith("[[")
									&& !clientSecrets.getDetails()
											.getClientSecret().startsWith("[["),
							"Please enter your client ID and secret from the Google APIs Console in %s from the "
									+ "root samples directory",
							RESOURCE_LOCATION);
		}
		return clientSecrets;
	}

	static String getRedirectUri(HttpServletRequest req) {
		GenericUrl url = new GenericUrl(req.getRequestURL().toString());
		url.setRawPath("/oauth2callback");
		return url.build();
	}

	static GoogleAuthorizationCodeFlow newFlow() throws IOException {
		return new GoogleAuthorizationCodeFlow.Builder(HTTP_TRANSPORT,
				JSON_FACTORY, getClientCredential(),
				Collections.singleton(CalendarScopes.CALENDAR))
				.setCredentialStore(new AppEngineCredentialStore()).build();
	}

	static Calendar loadCalendarClient() throws IOException {
		String userId = UserServiceFactory.getUserService().getCurrentUser()
				.getUserId();
		Credential credential = newFlow().loadCredential(userId);
		return Calendar.builder(HTTP_TRANSPORT, JSON_FACTORY)
				.setHttpRequestInitializer(credential).build();
	}

	/**
	 * Returns an {@link IOException} (but not a subclass) in order to work
	 * around restrictive GWT serialization policy.
	 */
	static IOException wrappedIOException(IOException e) {
		if (e.getClass() == IOException.class) {
			return e;
		}
		return new IOException(e.getMessage());
	}

	static PersistenceManager getPersistenceManager() {
		return pmfInstance.getPersistenceManager();
	}

	private Utils() {
	}
}
