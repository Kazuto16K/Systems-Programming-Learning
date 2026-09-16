#include<iostream>
#include<fstream>
#include<vector>
#include<cstring>

struct Record{
    uint32_t id;
    float score;
    char name[32];
};

/*
std::ofstream -> we're opening the file for output/writing
std::ios::binary -> open it in binary mode

std::ofstream out("records.txt"); <- This is normal text mode
std::ofstream out("records.bin", std::ios::binary);  <- This is in binary mode
*/

/*

ostream& write(const char* data, std::streamsize count);
MEANS: write(WHERE_DO_BYTES_START, HOW_MANY_BYTES)

Simpler: Starting at the memory address data, take 5 bytes and put them into the file.

*/

void writeRecord(const std::string& filename, const Record* record, size_t count){
    std::ofstream out(filename, std::ios::binary);

    if(!out){
        throw std::runtime_error("Cannot access file!");
    }

    out.write(
        reinterpret_cast<const char*>(record),
        count*sizeof(Record)
    );
}

/*

istream& read(char* destination, std::streamsize count);
MEANS: read(WHERE_TO_PUT_BYTES, HOW_MANY_BYTES)

Simpler: Take 5 bytes from the file and put them into buffer.

*/

std::vector<Record> readRecord(const std::string& filename, size_t count){
    std::ifstream in(filename, std::ios::binary);

    if(!in){
        throw std::runtime_error("Cannot open file");
    }

    std::vector<Record> records(count);

    // Take sizeof(Record) bytes from the file and copy them starting at the memory address of record.data().
    // reinterpret_cast means: "Treat this same memory address as a char*."

    in.read(
        reinterpret_cast<char*>(records.data()),
        count*sizeof(Record)
    );

    if(in.gcount()!= static_cast<std::streamsize>(count*sizeof(Record)) ){
        throw std::runtime_error("incomplete file!");
    }

    return records;
}

int main(){
    Record *original = new Record[3];
    original[0] = Record{1, 95.5f, "Soumava"};
    original[1] = Record{2, 87.2f, "Das"};
    original[2] = Record{3, 91.8f, "Github"};

    size_t count=3;
    writeRecord("records.bin", original, count);
    auto loaded = readRecord("records.bin", count);

    bool equal = std::memcmp(
        original,
        loaded.data(),
        count*sizeof(Record)
    ) == 0;

    delete[] original;

    std::cout<< (equal? "PASS\n":"FAIL\n");
    return 0;
}
    