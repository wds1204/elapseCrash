package com.modi.elapse.crash;

/**
 * @author wudongsheng
 */
public class UserManager {

    User user;

    public void setUser(User user) {
        this.user = user;
    }

    @Override
    public String toString() {
        return "UserManager{" +
                "user=" + user +
                '}';
    }
}
