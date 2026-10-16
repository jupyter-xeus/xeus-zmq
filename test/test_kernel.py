import time
import unittest
import jupyter_kernel_test
import zmq
from jupyter_client.manager import start_new_kernel


class XeusKernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "test_kernel"
    language_name = "cpp"

    code_hello_world = "hello, world"
    code_page_something = "?"

    code_execute_result = [
        {'code': '6*7', 'result': '6*7'},
        {'code': 'test', 'result': 'test'}
    ]

    completion_samples = [
        {'text': 'a.', 'matches': ['a.test1', 'a.test2']}
    ]

    complete_code_samples = ["complete"]
    incomplete_code_samples = ["incomplete"]
    invalid_code_samples = ["invalid"]

    code_inspect_sample = "invalid"

    def test_xeus_stderr(self):
        reply, output_msgs = self.execute_helper(code='error')
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stderr')
        self.assertEqual(output_msgs[0]['content']['text'], 'error')

class XeusIopubWelcomeTests(jupyter_kernel_test.IopubWelcomeTests):

    kernel_name = "test_kernel"
    support_iopub_welcome = True

class XeusIopubUpstreamMessagesTests(unittest.TestCase):

    kernel_name = "test_kernel"

    @classmethod
    def setUpClass(cls):
        cls.km, cls.kc = start_new_kernel(kernel_name=cls.kernel_name)

    @classmethod
    def tearDownClass(cls):
        cls.kc.stop_channels()
        cls.km.shutdown_kernel()

    def test_non_subscription_messages_are_ignored(self):
        # Besides subscription events, XPUB sockets pass along any message
        # sent by XSUB peers. These shouldn't make the kernel exit.
        socket = zmq.Context.instance().socket(zmq.XSUB)
        socket.linger = 0
        socket.connect(self.km._make_url("iopub"))
        try:
            socket.send_multipart([b"not", b"a subscription"])
            socket.send(b"")
            socket.send(b"\x02not a subscription either")
            time.sleep(1)
        finally:
            socket.close()

        self.assertTrue(self.km.is_alive())

        msg_id = self.kc.execute("6*7")
        reply = self.kc.get_shell_msg(timeout=10)
        self.assertEqual(reply["parent_header"]["msg_id"], msg_id)
        self.assertEqual(reply["content"]["status"], "ok")

        # iopub still works too
        while True:
            msg = self.kc.get_iopub_msg(timeout=10)
            if msg["parent_header"].get("msg_id") == msg_id and msg["msg_type"] == "execute_result":
                break


if __name__ == '__main__':
    unittest.main()
