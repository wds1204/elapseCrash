package com.modi.elapse.crash;

/**
 * @author wudongsheng
 */
public class User {
    private String name;
    private String age;

    public User(String name, String age) {
        this.name = name;
        this.age = age;
    }

    @Override
    public String toString() {
        return "User{" +
                "name='" + name + '\'' +
                ", age='" + age + '\'' +
                '}';
    }

    static User getUser() {
        return new User("wds", "18");
    }
}
