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
void BM_parse(benchmark::State &s, ExtraArgs&&... extra_args)
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

BENCHMARK_CAPTURE(BM_parse, many_double, "canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_parse, many_int32, "citm_catalog.json")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()