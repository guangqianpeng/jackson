//
// Created by frank on 17-12-27.
//

#include <benchmark/benchmark.h>

#include <jackson/Document.h>
#include <jackson/FileReadStream.h>
#include <jackson/StringWriteStream.h>
#include <jackson/Writer.h>
#include <jackson/PrettyWriter.h>


template <class ...ExtraArgs>
void BM_read(benchmark::State &s, ExtraArgs &&... extra_args)
{
    for (auto _: s) {
        FILE *input = fopen(extra_args..., "r");
        if (input == nullptr)
            exit(1);
        json::FileReadStream is(input);
        benchmark::DoNotOptimize(is);
        fclose(input);
    }
}

template <class ...ExtraArgs>
void BM_read_parse(benchmark::State &s, ExtraArgs &&... extra_args)
{
    for (auto _: s) {
        FILE *input = fopen(extra_args..., "r");
        if (input == nullptr)
            exit(1);
        json::Document doc;
        json::FileReadStream is(input);
        fclose(input);
        if (doc.parseStream(is) != json::PARSE_OK) {
            exit(1);
        }
    }
}

template <class ...ExtraArgs>
void BM_read_parse_write(benchmark::State &s, ExtraArgs&&... extra_args)
{
    for (auto _: s) {
        FILE *input = fopen(extra_args..., "r");
        if (input == nullptr)
            exit(1);
        json::Document doc;
        json::FileReadStream is(input);
        fclose(input);
        if (doc.parseStream(is) != json::PARSE_OK) {
            exit(1);
        }
        json::StringWriteStream os;
        json::Writer writer(os);
        json::writeTo(doc, writer);
        std::string_view ret = os.get();
        benchmark::DoNotOptimize(ret);
    }
}

//BENCHMARK_CAPTURE(BM_read, many_double, "canada.json")->Unit(benchmark::kMillisecond);
//BENCHMARK_CAPTURE(BM_read_parse, many_double, "canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_read_parse_write, many_double, "canada.json")->Unit(benchmark::kMillisecond);

//BENCHMARK_CAPTURE(BM_read, simple, "citm_catalog.json")->Unit(benchmark::kMillisecond);
//BENCHMARK_CAPTURE(BM_read_parse, simple, "citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_read_parse_write, simple, "citm_catalog.json")->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN()