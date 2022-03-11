"""MessageU-Server

The role of this module is to manage the list of users registered to
MessageU service and allow them to exchange messages of various types.

"""
import socket
import protocol
import threading
import database
import server_helper
from protocol_request import RequestCode
from protocol_response import ResponseCode

SERVER_VERSION = 2
UID_LEN = 16


def request_handler(conn, lock):
    db = database.DataBase()
    request = protocol.Request()
    response = protocol.Response(SERVER_VERSION)

    while True:
        try:
            uid = conn.recv(UID_LEN)
            if not uid:
                print('Connection closed by client.')
                break
        except Exception as connection_exception:
            print('Connection closed by client: %s' % connection_exception)
            break

        code = protocol.recv_and_decode_client_request(conn, db, request, uid)
        if code == ResponseCode.GENERAL_ERROR.value:
            response.set_general_error()

        elif code == RequestCode.REGISTER_REQUEST.value:
            if db.is_client_exists(request.payload.name):
                response.set_general_error()
            else:
                lock.acquire()
                uid = db.add_client(request.payload.name, request.payload.public_key)
                lock.release()
                response.set_registered_successfully(uid)

        elif code == RequestCode.CLIENTS_LIST_REQUEST.value:
            clients_list = db.get_clients_list(request.header.client_id)
            response.set_clients_list(clients_list)

        elif code == RequestCode.PUBLIC_KEY_REQUEST.value:
            if not db.is_id_exists(request.payload.client_id):
                response.set_general_error()
            else:
                public_key, = db.get_public_key(request.payload.client_id)
                response.set_public_key(public_key)

        elif code == RequestCode.PULL_MESSAGES_REQUEST.value:
            lock.acquire()
            messages = db.pull_messages(request.header.client_id)
            db.delete_messages(request.header.client_id)
            lock.release()
            response.set_pull_messages(messages)

        elif code == RequestCode.PUSH_MESSAGE_REQUEST.value:
            if not db.is_id_exists(request.payload.client_id):
                response.set_general_error()
            else:
                lock.acquire()
                message_id = db.add_message(request.payload.client_id,
                                            request.header.client_id,
                                            request.payload.message_type,
                                            request.payload.message_content)
                lock.release()
                response.set_push_message(request.payload.client_id, message_id)

        lock.acquire()
        db.update_client_last_seen(request.header.client_id)
        lock.release()

        encoded_response = protocol.encode_server_response(response)
        conn.sendall(encoded_response)


if __name__ == '__main__':
    TCP_IP = ''
    TCP_PORT = server_helper.get_tcp_port('port.info')

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.bind((TCP_IP, TCP_PORT))
            th_lock = threading.Lock()
            while True:
                sock.listen(100)
                sock_conn, address_and_port = sock.accept()
                client_thread = threading.Thread(target=request_handler, args=(sock_conn, th_lock))
                client_thread.start()
    except Exception as e:
        print('Error: %s' % e)