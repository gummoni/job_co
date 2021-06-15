#include <stdio.h>
#include <stdlib.h>



enum class job_state {
	START = 0,
	BUSY = 1,
	SUCCESS = 2,
	RETRY = 3,
	CANCEL = 4,
	ERROR = 5,
};

/// <summary>
/// ジョブ
/// </summary>
class job {
private:
	virtual void start(void* args) = 0;
	virtual void busy(void* args) = 0;
	virtual void retry(void* args) = 0;
	virtual void success(void* args) = 0;
	virtual void cancel(void* args) = 0;
	virtual void failed(void* args) = 0;

	job_state state;
	job* next;

	/// <summary>
	/// 最後の位置
	/// </summary>
	/// <returns></returns>
	job* last() {
		job* cur = this;
		while (NULL != cur->next)
			next = cur->next;

		return cur;
	}

protected:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	job() {
		state = job_state::START;
		next = NULL;
	}

	/// <summary>
	/// 継続
	/// </summary>
	void yield(job_state state) {
		this->state = state;
		last()->next = this;
	}

	/// <summary>
	/// ユーザ問合せ
	/// </summary>
	/// <param name="error_code"></param>
	void user_operation(int error_code) {
		throw error_code;
	}

public:

	/// <summary>
	/// 処理移譲
	/// </summary>
	/// <param name="job"></param>
	job* invoke(job* job) {
		last()->next = job;
		job->next = NULL;
		return job;
	}

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="args"></param>
	/// <returns></returns>
	job_state execute(void* args) {
		job* cur = this;
		while (NULL != cur) {
			switch (cur->state) {
			case job_state::START:
				cur->start(args);
				break;

			case job_state::BUSY:
				cur->busy(args);
				break;

			case job_state::SUCCESS:
				cur->success(args);
				break;

			case job_state::RETRY:
				cur->retry(args);
				break;

			case job_state::CANCEL:
				cur->cancel(args);
				break;

			case job_state::ERROR:
			default:
				cur->failed(args);
				break;
			}

			//次へ
			job* tmp = cur->next;
			cur->next = NULL;
			cur = tmp;
		}
		return state;
	}
};

class job2 : public job {
private:
	int count;

	virtual void start(void* args) override
	{
		printf("start\n");
		yield(job_state::BUSY);
	}

	virtual void busy(void* args) override
	{
		if (count++ < 10) {
			printf("busy: %d\n", count);
			yield(job_state::BUSY);
		}
		else
			yield(job_state::SUCCESS);
	}

	virtual void retry(void* args) override
	{
	}

	virtual void success(void* args) override
	{
		printf("success\n");
	}

	virtual void cancel(void* args) override
	{
	}

	virtual void failed(void* args) override
	{
	}

public:
	job2(int count) : job() {
		this->count = count;
	}
};

class job1 : public job {
private:
	int count;

	virtual void start(void* args) override
	{
		printf("start\n");
		yield(job_state::BUSY);
	}

	virtual void busy(void* args) override
	{
		if (count++ < 10) {
			printf("busy: %d\n", count);
			yield(job_state::BUSY);
		}
		else
			yield(job_state::SUCCESS);
	}

	virtual void retry(void* args) override
	{
	}

	virtual void success(void* args) override
	{
		printf("success\n");
		invoke(new job2(-5));
	}

	virtual void cancel(void* args) override
	{
	}

	virtual void failed(void* args) override
	{
	}

public:
	job1(int count) : job() {
		this->count = count;
	}
};

int main(void) {
	job1* j1 = new job1(-10);

	j1->execute(NULL);
}
