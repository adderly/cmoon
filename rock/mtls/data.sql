-- reqtype 0: html, 1: ajax
-- lmttype 0: none, 1: LMT_TYPE_MEMBER, 2: LMT_TYPE_GJOIN, 3 LMT_TYPE_JUNIOR, 4 LMT_TYPE_SENIOR, 5 LMT_TYPE_GADM, 6 LMT_TYPE_GOWN, 255: LMT_TYPE_ROOT
-- 0001' 0000'0001' 0000'0000 65792
-- 0101' 0000'0001' 0000'0001 327937
-- 1111' 0000'0111' 0000'0001 984833
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (0, 1001, 1, 0, 0, '/', '��ҳ'); -- can't direct access

INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (1, 1001, 65792, 0, 0, 'admin', '����'); -- can't direct access
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (1, 1001, 1, 0, 0, 'member', '�û�'); -- can't direct access
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (1, 1001, 1, 0, 0, 'service', '����'); -- can't direct access
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (1, 1001, 327937, 0, 0, 'csc', '���г�');

INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (2, 1001, 65792, 1, 1, 'profile', '��Ϣ');
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (2, 1001, 65792, 1, 255, 'account', '�ʺ�');
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (2, 1001, 65792, 1, 4, 'file', '�ļ�');
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (2, 1001, 65792, 1, 3, 'group', '��Ա');

INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (3, 1001, 1, 1, 0, 'login', '��¼');
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (3, 1001, 1, 1, 0, 'logout', '�ǳ�');
INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (3, 1001, 1, 1, 0, 'regist', 'ע��');

INSERT INTO fileinfo (pid, uid, mode, reqtype, lmttype, name, remark) VALUES (4, 1001, 65792, 1, 0, 'action', '����');
