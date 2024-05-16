#define CONTACT_SUCCESS 0
#define CONTACT_FAILURE 1

struct Student {
    int student_ID;
    char name[50];
    char phone[50];
};

struct Course {
    int course_ID;
    char course_Name[100];
};

extern struct PDS_RepoInfo *repoHandle;