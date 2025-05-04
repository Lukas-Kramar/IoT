import React, { useEffect, useState } from 'react';
import { Form, Button, Row, Col, Container, Alert } from 'react-bootstrap';
import { useNavigate, useParams } from 'react-router-dom';

import { useLoggedUserContext } from '../../customHooks/useLoggedUserContext';
import { Policy, User } from '../../../API/requests/userRequests';

const UserProfile = () => {
    const { userId } = useParams();
    const navigate = useNavigate();

    const { userData } = useLoggedUserContext();
    const [editedUser, setEditedUser] = useState<User | null>(userData);

    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
        // TODO - handle form submission logic
        // Implement the save logic here, e.g., sending the updated user data to the server
        console.log('User data saved:', editedUser);
    };

    useEffect(() => {
        if (typeof userId !== "string") { return; }
        // setEditedUser(users.find((user) => user.id === userId) ?? null)
    }, [userId]);

    return (
        <Container className=''>
            <h1 className='my-3'>User Profile</h1>
            {!editedUser
                ? (
                    <>
                        <Alert>User with this ID doesn't exists </Alert>
                        <Button onClick={() => navigate("/")}>Go back to Dashboard</Button>
                    </>
                )
                : (
                    <Form onSubmit={handleSubmit}>
                        <Row className="mb-3">
                            <Form.Group as={Col} controlId="formName">
                                <Form.Label>First Name</Form.Label>
                                <Form.Control
                                    type="text"
                                    name="name"
                                    value={editedUser.firstName}
                                    onChange={(e) => setEditedUser({ ...editedUser, firstName: e.target.value })}
                                />
                            </Form.Group>
                        </Row>
                        {/* <Form.Group controlId="formPassword" className="mb-3">
                            <Form.Label>Password</Form.Label>
                            <Form.Control
                                type="password"
                                name="password"
                                value={editedUser.password}
                                onChange={handleChange}
                            />
                        </Form.Group> */}
                        {/* TODO - if currentLogedUser role === "ADMIN" */}
                        <Row>
                            <Col sm={4}>
                                {(
                                    <Form.Group controlId="formRole" className="mb-3">
                                        <Form.Label>Role</Form.Label>
                                        <Form.Select
                                            disabled={!userData || userData.role !== Policy.Admin || userData?._id === editedUser._id}
                                            name="role"
                                            value={editedUser.role}
                                            onChange={(e) => {
                                                const value = Number(e.target.value);
                                                setEditedUser({ ...editedUser, role: value === Policy.Admin ? Policy.Admin : Policy.Member });
                                            }}
                                        >
                                            <option value={Policy.Admin}>Admin</option>
                                            <option value={Policy.Member}>Member</option>
                                        </Form.Select>
                                    </Form.Group>
                                )}
                            </Col>
                            <Col sm={4}>
                            </Col>
                            <Col sm={4} className='d-flex align-items-end justify-content-end'>
                                <Button variant="primary" type="submit">
                                    Save
                                </Button>
                            </Col>
                        </Row>


                    </Form>
                )
            }
        </Container>
    );
};

export default UserProfile;