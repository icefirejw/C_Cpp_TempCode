#include <iostream>

using namespace std;

typedef int ElemType;

template <class T>          //定义获取数组大小的模板
int getlen(T& array)
{
    return (sizeof(array) / sizeof(array[0]));
}
void swap(ElemType* a, ElemType* b)
{
    ElemType temp;

    temp = *b;
    *b = *a;
    *a = temp;
}

void quick_sort(ElemType array[], int left, int right)
{
    int i,j;
    ElemType head;
    
    if (left >= right)
        return;
    
    i = left;
    j = right;
    
    head = array[left];

    while (i<j)
    {
        //must be right first
        while(array[j] >= head && i < j ) 
            j--;

        while(array[i] <= head && i < j) 
            i++;

        if (i >= j)
            break;

        swap(&array[i], &array[j]);
    }

    swap(&array[left], &array[i]); 

    quick_sort(array, left, i-1);
    quick_sort(array, i+1, right);

    return 0;
}

void quickSort(ElemType s[], int l, int r)
{
    int i = l, j = r;
    ElemType x = s[l];

    if (l >= r)
        return;

    while (i < j)
    {
        while(i < j && s[j]>= x) // 从右向左找第一个小于x的数
            j--; 
        if(i < j)
            s[i++] = s[j];

        while(i < j && s[i]< x) // 从左向右找第一个大于等于x的数
            i++;
        if(i < j)
            s[j--] = s[i];
    }
    s[i] = x;
    quickSort(s, l, i - 1); // 递归调用
    quickSort(s, i + 1, r);
}


int main(int argc, char* argr[])
{
    int arr[] = {20,10,29,387,23,7,14,20,99,87,25,46};

    for (int i = 0; i<getlen(arr); i++)
        cout << arr[i] << " " ;

    
    quickSort(arr, 0, getlen(arr)-1);

    cout << "\nsorted:" <<endl;
    for (int i = 0; i<getlen(arr); i++)
        cout << arr[i] << " ";

    getchar();
    
    return 0;
}



