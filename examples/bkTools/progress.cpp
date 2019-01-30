/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>

#include <bk/Progress>

void on_task_finished(unsigned int taskId)
{
    std::cout << "task finished (id " << taskId << ")" << std::endl;
}

void on_task_added(unsigned int taskId, double maxProgress, double currentProgress, std::string_view description)
{
    std::cout << "task added (id " << taskId << ", \"" << description << "\") ; progress " << currentProgress << " / " << maxProgress << std::endl;
}

void on_progress_changed(double currentProgress)
{
    // do nothing
}

int main(int, char**)
{
    std::cout << std::fixed;

    bk::ProgressManager progman;

    // connect to functions with signature of the corresponding signals
    progman.signal_task_added().connect(&on_task_added);
    progman.signal_task_finished().connect(&on_task_finished);

    // alternative: connect to lambda function
    //progman.signal_task_finished().connect([](unsigned int taskId) { on_task_finished(taskId); });

    int N = 1'000'000;

    bk::Progress& prog = progman.emplace_task(N, "my calculation"); // add progress tracker for a task
    prog.signal_current_changed().connect(&on_progress_changed); // call function every time progress changes

    std::cout << "number of active tasks: " << progman.num_tasks() << std::endl;

    for (int i = 0; i < N; ++i)
    {
        /* do stuff */

        prog.increment(1);
    }

    std::cout << "number of active tasks: " << progman.num_tasks() << std::endl << std::endl;

    /* output:
     *    task added (id 0, "my calculation") ; progress 0.000000 / 1000000.000000
     *    number of active tasks: 1
     *    task finished (id 0)
     *    number of active tasks: 0
     */

    // ----------------------------------------------------------------------

    bk::Progress& progParallel = progman.emplace_task(N, "my parallel calculation");

    #pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        /* do stuff (parallel) */

        #pragma omp critical
        { progParallel.increment(1); }
    }

    /* output:
     *    task added (id 1, "my parallel calculation") ; progress 0.000000 / 1000000.000000
     *    task finished (id 1)
     */

    return EXIT_SUCCESS;
}

