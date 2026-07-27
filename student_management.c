#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LECTURES 5
#define NAME_LENGTH 50

typedef enum {
    CSE101, CSE102, CSE103, CSE104, CSE105, INVALID_LECTURE
} Lecture;

const char *lecture_names[] = {"CSE101", "CSE102", "CSE103", "CSE104", "CSE105"};
const int lecture_credits[] = {2, 3, 3, 4, 5};

typedef struct Student {
    char name[NAME_LENGTH];
    int student_id;
    Lecture lectures[MAX_LECTURES];
    int grades[MAX_LECTURES];
    int lecture_count;
    float gpa;
    struct Student *next;
} Student;

Student *head = NULL;
int next_id = 1;

char* trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t') str++;
    return str;
}

Lecture get_lecture_enum(const char *str) {
    int i;
    for (i = 0; i < MAX_LECTURES; i++) {
        if (strcmp(str, lecture_names[i]) == 0)
            return (Lecture)i;
    }
    return INVALID_LECTURE;
}

int is_valid_grade(int grade) {
    return grade >= 0 && grade <= 100;
}

int validate_lecture_grade_input(char *lecture_input, char *grade_input) {
    char lecture_copy[200], grade_copy[200];
    char *lec, *grd;
    Lecture l;
    int grade;

    strcpy(lecture_copy, lecture_input);
    strcpy(grade_copy, grade_input);
    
    lec = strtok(lecture_copy, ",");
    grd = strtok(grade_copy, ",");

    while (lec && grd) {
        lec = trim_whitespace(lec);
        grd = trim_whitespace(grd);

        l = get_lecture_enum(lec);
        if (l == INVALID_LECTURE) {
            printf("Error: Lecture '%s' is invalid.\n", lec);
            return 0;
        }

        grade = atoi(grd);
        if (!is_valid_grade(grade)) {
            printf("Error: Grade '%s' is invalid (must be 0-100).\n", grd);
            return 0;
        }

        lec = strtok(NULL, ",");
        grd = strtok(NULL, ",");
    }

    if ((lec && !grd) || (!lec && grd)) {
        printf("Error: Number of lectures and grades must match.\n");
        return 0;
    }

    return 1;
}

void calculate_gpa(Student *student) {
    int total = 0, credit_sum = 0;
    int i;
    for (i = 0; i < student->lecture_count; i++) {
        total += student->grades[i] * lecture_credits[student->lectures[i]];
        credit_sum += lecture_credits[student->lectures[i]];
    }
    student->gpa = (credit_sum == 0) ? 0 : (float)total / credit_sum;
}

void print_ids() {
    Student *curr = head;
    printf("id : ");
    while (curr) {
        printf("%d", curr->student_id);
        if (curr->next) printf(",");
        curr = curr->next;
    }
    printf("\n");
}

Student *find_student(int id) {
    Student *curr = head;
    while (curr) {
        if (curr->student_id == id)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void print_student_details(Student *student) {
    int i;
    printf("-------------------------------------------------------------------------------------------\n");
    printf("Name\t\tLectures\t\t\tGrades\t\tGPA\n");
    printf("-------------------------------------------------------------------------------------------\n");
    printf("%s\t\t", student->name);
    for (i = 0; i < student->lecture_count; i++) {
        printf("%s", lecture_names[student->lectures[i]]);
        if (i < student->lecture_count - 1) printf(", ");
    }
    printf("\t\t");
    for (i = 0; i < student->lecture_count; i++) {
        printf("%d", student->grades[i]);
        if (i < student->lecture_count - 1) printf(", ");
    }
    printf("\t\t%.2f\n", student->gpa);
}

void add_student() {
    char lecture_input[200], grade_input[200];
    char name_input[NAME_LENGTH];
    Student *new_student;
    char *lec, *grd;
    Lecture l;

    printf("Enter student name: ");
    scanf(" %[^\n]", name_input);

    printf("Enter lectures: ");
    scanf(" %[^\n]", lecture_input);
    printf("Enter grades: ");
    scanf(" %[^\n]", grade_input);

    if (!validate_lecture_grade_input(lecture_input, grade_input)) {
        printf("Student not added due to invalid input.\n");
        return;
    }

    new_student = (Student *)malloc(sizeof(Student));
    if (!new_student) {
        printf("Memory allocation failed!\n");
        return;
    }

    new_student->student_id = next_id++;
    new_student->next = head;
    new_student->lecture_count = 0;
    strcpy(new_student->name, name_input);

    lec = strtok(lecture_input, ",");
    grd = strtok(grade_input, ",");

    while (lec && grd && new_student->lecture_count < MAX_LECTURES) {
        lec = trim_whitespace(lec);
        grd = trim_whitespace(grd);

        l = get_lecture_enum(lec);
        new_student->lectures[new_student->lecture_count] = l;
        new_student->grades[new_student->lecture_count] = atoi(grd);
        new_student->lecture_count++;

        lec = strtok(NULL, ",");
        grd = strtok(NULL, ",");
    }

    calculate_gpa(new_student);
    head = new_student;
    printf("Student added.\n");
    print_ids();
}

void remove_student() {
    int id;
    Student *curr = head;
    Student *prev = NULL;

    printf("Enter student id: ");
    scanf("%d", &id);
    while (curr && curr->student_id != id) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) {
        printf("Student not found.\n");
        return;
    }
    if (!prev) head = curr->next;
    else prev->next = curr->next;
    free(curr);
    printf("Student was removed.\n");
    print_ids();
}

void search_student() {
    int id;
    Student *student;
    printf("Enter student id: ");
    scanf("%d", &id);
    student = find_student(id);
    if (!student) {
        printf("Student not found.\n");
        return;
    }
    print_student_details(student);
}

void add_lecture() {
    int id, grade, updated, i;
    char lecture_input[200], grade_input[200];
    char *lec, *grd;
    Lecture l;
    Student *student;

    printf("Enter student id: ");
    scanf("%d", &id);
    student = find_student(id);
    if (!student) {
        printf("Student not found.\n");
        return;
    }

    printf("Enter lectures: ");
    scanf(" %[^\n]", lecture_input);
    printf("Enter grades: ");
    scanf(" %[^\n]", grade_input);

    if (!validate_lecture_grade_input(lecture_input, grade_input)) {
        printf("Lectures not added due to invalid input.\n");
        return;
    }

    lec = strtok(lecture_input, ",");
    grd = strtok(grade_input, ",");

    while (lec && grd) {
        lec = trim_whitespace(lec);
        grd = trim_whitespace(grd);

        l = get_lecture_enum(lec);
        grade = atoi(grd);
        updated = 0;

        for (i = 0; i < student->lecture_count; i++) {
            if (student->lectures[i] == l) {
                student->grades[i] = grade;
                updated = 1;
                break;
            }
        }

        if (!updated && student->lecture_count < MAX_LECTURES) {
            student->lectures[student->lecture_count] = l;
            student->grades[student->lecture_count++] = grade;
        }

        lec = strtok(NULL, ",");
        grd = strtok(NULL, ",");
    }

    calculate_gpa(student);
    print_student_details(student);
}

void remove_lecture() {
    int id, i, j;
    char lecture_input[200];
    char *lec;
    Lecture l;
    Student *student;

    printf("Enter student id: ");
    scanf("%d", &id);
    student = find_student(id);
    if (!student) {
        printf("Student not found.\n");
        return;
    }

    printf("Enter lectures: ");
    scanf(" %[^\n]", lecture_input);

    lec = strtok(lecture_input, ",");
    while (lec) {
        lec = trim_whitespace(lec);
        l = get_lecture_enum(lec);
        if (l == INVALID_LECTURE) {
            printf("Error: Lecture '%s' is invalid.\n", lec);
            lec = strtok(NULL, ",");
            continue;
        }

        for (i = 0; i < student->lecture_count; i++) {
            if (student->lectures[i] == l) {
                for (j = i; j < student->lecture_count - 1; j++) {
                    student->lectures[j] = student->lectures[j + 1];
                    student->grades[j] = student->grades[j + 1];
                }
                student->lecture_count--;
                break;
            }
        }

        lec = strtok(NULL, ",");
    }

    calculate_gpa(student);
    print_student_details(student);
}

void print_list() {
    Student *curr = head;
    int i;
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("Name\t\tLectures\t\t\tGrades\t\tGPA\tID\n");
    printf("----------------------------------------------------------------------------------------------------\n");
    while (curr) {
        printf("%s\t\t", curr->name);
        for (i = 0; i < curr->lecture_count; i++) {
            printf("%s", lecture_names[curr->lectures[i]]);
            if (i < curr->lecture_count - 1) printf(", ");
        }
        printf("\t\t");
        for (i = 0; i < curr->lecture_count; i++) {
            printf("%d", curr->grades[i]);
            if (i < curr->lecture_count - 1) printf(", ");
        }
        printf("\t\t%.2f\t%d\n", curr->gpa, curr->student_id);
        curr = curr->next;
    }
}

void free_memory() {
    Student *temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    int choice;
    do {
        printf("\nMenu\n");
        printf("1. Add Student\n");
        printf("2. Remove Student\n");
        printf("3. Search Student\n");
        printf("4. Add Lecture\n");
        printf("5. Remove Lecture\n");
        printf("6. Print ID\n");
        printf("7. Print List\n");
        printf("8. Exit\n");
        printf("Choice: ");
        
        scanf("%d", &choice);
        switch (choice) {
            case 1: add_student(); break;
            case 2: remove_student(); break;
            case 3: search_student(); break;
            case 4: add_lecture(); break;
            case 5: remove_lecture(); break;
            case 6: print_ids(); break;
            case 7: print_list(); break;
            case 8: printf("Freeing memory... Have a nice day!\n"); free_memory(); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 8);
    return 0;
}
